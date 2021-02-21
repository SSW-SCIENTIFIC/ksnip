/*
 * Copyright (C) 2020 Damir Porobic <damir.porobic@gmx.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "FileUrlHelper.h"

QString FileUrlHelper::toPath(const QString &url)
{
	auto path = url;
	return path.remove(filePrefix());
}

QString FileUrlHelper::toFileUrl(const QString &path)
{
	return filePrefix() + path;
}

QString FileUrlHelper::filePrefix()
{
#if defined(__APPLE__)
	return QLatin1String("file://");
#endif

#if defined(UNIX_X11)
	return QLatin1String("file://");
#endif

#if  defined(_WIN32)
	return QLatin1String("file:///");
#endif
}
