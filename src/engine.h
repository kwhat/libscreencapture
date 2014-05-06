/* libScreenShot: Cross-platfrom fast screenshot library.
 * Copyright (C) 2014 Alexander Barker.  All Rights Received.
 * https://github.com/kwhat/libscreenshot/
 *
 * libScreenShot is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libScreenShot is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _included_engine
#define _included_engine

extern int init();

extern int cleanup();

extern screenshot* capture();

#endif
