/*
 * Copyright (C) 2021 Damir Porobic <damir.porobic@gmx.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "CommandLineCaptureHandler.h"

CommandLineCaptureHandler::CommandLineCaptureHandler(
		const QSharedPointer<IImageGrabber> &imageGrabber,
		const QSharedPointer<IUploadHandler> &uploadHandler,
		const QSharedPointer<IImageSaver> &imageSaver,
		const QSharedPointer<ISavePathProvider> &savePathProvider) :
	mImageGrabber(imageGrabber),
	mUploadHandler(uploadHandler),
	mImageSaver(imageSaver),
	mSavePathProvider(savePathProvider),
	mIsWithSave(false),
	mIsWithUpload(false)
{
	connect(mImageGrabber.data(), &IImageGrabber::finished, this, &CommandLineCaptureHandler::processCapture);
	connect(mImageGrabber.data(), &IImageGrabber::canceled, this, &CommandLineCaptureHandler::canceled);

	connect(mUploadHandler.data(), &IUploader::finished, this, &CommandLineCaptureHandler::uploadFinished);
}

void CommandLineCaptureHandler::captureAndProcessScreenshot(const CommandLineCaptureParameter &parameter)
{
	mIsWithSave = parameter.isWithSave;
	mIsWithUpload = parameter.isWithUpload;
	mSavePath = parameter.savePath;
	mImageGrabber->grabImage(parameter.captureMode, parameter.isWithCursor, parameter.delay);
}

void CommandLineCaptureHandler::processCapture(const CaptureDto &capture)
{
	mCurrentCapture = capture;

	if (mIsWithSave) {
		saveCapture(mCurrentCapture);
	}

	if (mIsWithUpload) {
		mUploadHandler->upload(capture.screenshot.toImage());
	} else {
		finished(mCurrentCapture);
	}
}

void CommandLineCaptureHandler::saveCapture(const CaptureDto &capture)
{
	auto savePath = mSavePath.isEmpty() ? mSavePathProvider->savePath() : mSavePath;
	auto isSaveSuccessful = mImageSaver->save(capture.screenshot.toImage(), savePath);

	if (isSaveSuccessful) {
		qInfo("Capture saved to %s", qPrintable(savePath));
	} else {
		qWarning("Failed to save capture to %s", qPrintable(savePath));
	}
}

QList<CaptureModes> CommandLineCaptureHandler::supportedCaptureModes() const
{
	return mImageGrabber->supportedCaptureModes();
}

void CommandLineCaptureHandler::uploadFinished(const UploadResult &result)
{
	if (result.isError()) {
		auto enumTranslator = EnumTranslator::instance();
		qWarning("Upload failed: %s", qPrintable(enumTranslator->toString(result.status)));
	} else {
		qInfo("Upload finished");
	}

	if (result.hasContent()) {
		qInfo("Upload result: %s", qPrintable(result.content));
	}

	finished(mCurrentCapture);
}
