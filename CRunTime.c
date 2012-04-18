/***************************************************************************
 *                                                                         *
 *  This program is free software; you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation; either version 2 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program; if not, write to the Free Software            *
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111 USA    *
 *                                                                         *
 ***************************************************************************
 *                                                                         *
 *               (c) Copyright, 2001-2012, ANSR                            *
 *                                                                         *
 ***************************************************************************
 *                                                                         *
 * Filename:     CRunTime.c                                                *
 *                                                                         *
 * History:      v 1.0 - Initial release                                   *
 ***************************************************************************/

#include <sys/types.h>
#include <sys/stat.h>

// Since the CodeSourcery compiler is for a "bare metal" machine, the
// C library needs minimal support functions.

int _isatty(int fd)
{
    return 1;
}

int _lseek(int fd, int ptr,int dir)
{
    return -1;
}

int _read(int fd, void *buffer, unsigned int count)
{
    *((char *) buffer) = '*';

    return 1;
}

int _close(int fd)
{
    return 0;
}

int _write(int fd, const void *data, unsigned int count)
{
    return count;
}

int _fstat(int fd, struct stat * st)
{
    st->st_mode = S_IFCHR;
    return 0;
}

register char *stack_ptr asm ("sp");

caddr_t _sbrk_r(void *reent, size_t incr)
{
    // Defined by the linker
    extern char end asm ("end");

    static char *heap_end = NULL;

    char *prev_heap_end;

    if(heap_end == NULL)
        heap_end = &end;

    prev_heap_end = heap_end;

    //if(( heap_end + incr ) > stack_ptr )
        //return (caddr_t) -1;

    heap_end += incr;
    return (caddr_t) prev_heap_end;
}




