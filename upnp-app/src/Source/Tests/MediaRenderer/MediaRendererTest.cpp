/*****************************************************************
|
|   Platinum - Test UPnP A/V MediaRenderer
|
| Copyright (c) 2004-2010, Plutinosoft, LLC.
| All rights reserved.
| http://www.plutinosoft.com
|
| This program is free software; you can redistribute it and/or
| modify it under the terms of the GNU General Public License
| as published by the Free Software Foundation; either version 2
| of the License, or (at your option) any later version.
|
| OEMs, ISVs, VARs and other distributors that combine and
| distribute commercially licensed software with Platinum software
| and do not wish to distribute the source code for the commercially
| licensed software under version 2, or (at your option) any later
| version, of the GNU General Public License (the "GPL") must enter
| into a commercial license agreement with Plutinosoft, LLC.
| licensing@plutinosoft.com
|
| This program is distributed in the hope that it will be useful,
| but WITHOUT ANY WARRANTY; without even the implied warranty of
| MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
| GNU General Public License for more details.
|
| You should have received a copy of the GNU General Public License
| along with this program; see the file LICENSE.txt. If not, write to
| the Free Software Foundation, Inc.,
| 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
| http://www.gnu.org/licenses/gpl-2.0.html
|
 ****************************************************************/

/*----------------------------------------------------------------------
|   includes
+---------------------------------------------------------------------*/
#include "PltUPnP.h"
#include "PltMediaRenderer.h"
#include "Aml_MediaRenderDelegate.h"
#include <stdlib.h>
#include <csignal>
#include <unistd.h>
/*----------------------------------------------------------------------
|   globals
+---------------------------------------------------------------------*/
struct Options {
    const char *friendly_name;
} Options;

/*----------------------------------------------------------------------
|   PrintUsageAndExit
+---------------------------------------------------------------------*/
static void
PrintUsageAndExit( char **args )
{
    fprintf( stderr, "usage: %s [-f <friendly_name>]\n", args[0] );
    fprintf( stderr, "-f : optional upnp server friendly name\n" );
    fprintf( stderr, "<path> : local path to serve\n" );
    exit( 1 );
}
static bool added = false;
/*----------------------------------------------------------------------
|   ParseCommandLine
+---------------------------------------------------------------------*/
static void
ParseCommandLine( char **args )
{
    const char *arg;
    char      **tmp = args + 1;
    /* default values */
    Options.friendly_name = NULL;
    while ( ( arg = *tmp++ ) ) {
        if ( !strcmp( arg, "-f" ) ) {
            Options.friendly_name = *tmp++;
        } else {
            fprintf( stderr, "ERROR: too many arguments\n" );
            PrintUsageAndExit( args );
        }
    }
}
void handleStop( int sig )
{
    added = false;
    printf( "get Signal %d\n", sig );
    exit( -1 );
}
/*----------------------------------------------------------------------
|   main
+---------------------------------------------------------------------*/
int
main( int /* argc */, char **argv )
{
    NPT_LogManager::GetDefault().Configure( "plist:.level=INFO;.handlers=ConsoleHandler;.ConsoleHandler.outputs=2;.ConsoleHandler.colors=false;.ConsoleHandler.filter=59" );
    PLT_UPnP upnp;
    NPT_LOG_WARNING( "start" );
    /* parse command line */
    ParseCommandLine( argv );
    PLT_MediaRenderer render = PLT_MediaRenderer( Options.friendly_name ? Options.friendly_name : "Platinum Media Renderer",
                               false,
                               "e6572b54-f3c7-2d91-2fb5-b757f2537e21" );
    /*PLT_DeviceHostReference device(
         new PLT_MediaRenderer(Options.friendly_name?Options.friendly_name:"Platinum Media Renderer",
                               false,
                               "e6572b54-f3c7-2d91-2fb5-b757f2537e21"));*/
    PLT_MediaRendererDelegate *delegate = new Aml_MediaRenderDelegate();
    render.SetDelegate( delegate );
    PLT_DeviceHostReference device( &render );
    upnp.AddDevice( device );
    bool added = true;
    upnp.Start();
    signal( SIGINT, handleStop );
    signal( SIGTERM, handleStop );
    while ( added ) {
        sleep( 1000 );
    }
    /*char buf[256];
    while (gets(buf)) {
        if (*buf == 'q')
            break;

        if (*buf == 's') {
            if (added) {
                upnp.RemoveDevice(device);
            } else {
                upnp.AddDevice(device);
            }
            added = !added;
        }
    }*/
    upnp.RemoveDevice( device );
    upnp.Stop();
    delete render;
    return 0;
}
