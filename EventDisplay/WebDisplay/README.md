# EMPHATIC Web Display
A browser-based event display for the EMPHATIC experiment.

## Usage
1. Log in to an emphaticgpvm with a **special ssh flag**: `ssh -L 3490:localhost:3490 emphaticgpvm03.fnal.gov`
2. Set up to develop emphaticsoft normally.  You'll forward port 3490 through the SL7 container since you're already on this branch.
3. Find a file you want to read.  It must have **SSD LineSegments** from the MakeSSDClusters module since that's all the event display shows.
4. Start the "web server" ART job: `art -c webDisplay.fcl <yourFile.root>`
5. Open a web browser **on your desktop**.  Type into the URL bar: `localhost:3490`
6. The browser make take a few seconds to load the first event.  Then you should see a black screen with teal lines and some blue and red shapes:
  - Teal lines are SSD LineSegments
  - Red shapes are the magnet and the target
  - Blue shapes are the outlines of SSD sensors
7. How to interact with the browser:
  - Left click and drag to change your viewing angle
  - Right click and drag to change your viewing position in the plane you're viewing from
  - Scroll to zoom in or out
  - Reload the page to go to the next event

## Supported Platforms
- Your laptop (client side):
  - Firefox on Ubuntu (TODO: version.  Mine is pretty recent)
  - Chrome on Ubuntu (TODO: version.  Mine is very old)
  - Chrome on OS X
  - TODO: Safari on OS X?
- Server side:
  - Any POSIX OS on which you can compile emphaticsoft

## Details
Might be useful in case you need to debug something

- The event display just draws LineSegments and MC trajectories right now.  No tracks, hits, or Cherenkov information (yet!).  The magnet and target positions are from the Geometry service, but **the magnet size is not accurate** right now.
- The WebDisplay is acting as a trivial "web server" that sends and receives HTTP over a port
- You have to get port 3490 from the ART job to your web browser on your device.  In these instructions, it is forwarded by ssh which should even encrypt traffic over the port and requires you to authenticate using kerberos to use it effectively.  Not so friendly to non-collaborators, but hopefully overly safe since I'm a web neophyte.
- The 3D display is run client-side (on your computer, not the GPVM) using a Javascript library called three.js.  I got the idea to use this library from CMS's outreach event display.
- The GPVM "web server" is just a code generator that fills in some Javascript based on the LineSegments in the display and the Geometry service.
- We require a POSIX OS (i.e. not Windows) for 2 reasons that I know of: the sockets API, and the stat() API.  stat() is easy to replace with ROOT shenanigans that I don't like from TSystem.  ROOT and Boost may have more robust socket implementations.  The only reason I didn't use a portable socket implementation was to save myself work on the demonstration.

## TODOs
- Before next release:
  - Does this work on MacOS + Safari?  It worked for Aayush on MacOS + Chrome.
- Before next collaboration meeting:
  - Better default camera positions and buttons to switch between them!
  - Better camera controls?  What do collaborators think of these OrbitControls?  Would others like arrow key navigation, or is the camera good enough as is?
  - Do we have reconstructed SSD tracks?
  - Add a list tree GUI?  It might help the MC make more sense, but I've always found them to be clutter to the majority of use cases.
  - Why are the downstream SSDs in weird positions?  Is something wrong with their units again, or is there a problem on the Javascript side?
- Before next run:
- Later...
  - Port this to a real web server running e.g. at Notre Dame?  That would be nice for outreach!  But then someone has to understand and maintain a web server application.  The webserver I'm imagining would produce a web page that looks like Nathaniel Tagg's Arachne event display for MINERvA.  We could make an ART job act like a "CGI script" by reading requests on STDIN and replying on STDOUT.  It even sounds like we could leave the ART job running between serving events with Apache.  When I searched for ideas using Google, I saw recommendations that we might as well write an Apache module if we're forced to use C.  I'm not sure how to do that, and it sounds like more dependencies to me.
  - Display accurate magnet geometry from GDML file.  Andrew has code that turns a TGeoShape into a list of vertices that THREE.js can use.  I still have to figure out how I want to stream them.  As a .obj (Wavefront) file?  Directly as an HTTP blob?  Using a socket directly in Javascript?
  - Persist camera settings across events.  I could either cache them somehow or do a massive redesign of the Javascript so that I only send positions and rotations for each event but keep the rendering code.  The latter probably makes us more robust in the long run, but that's a lot of work!  And a lot of things I don't yet know how to do in Javascript.
  - How often do we have to update the Javascript side of this application?  How often does the THREE.js API change?  Not work with certain browsers?  As a Javascript newcomer, I don't have easy answers to these questions.  CMS made it work somehow, so I have some hope this won't be too bad...
  - Random access to events.  I might end up sending and parsing web forms.  Some good resources to get me started:
    - https://developer.mozilla.org/en-US/docs/Web/HTTP/Methods
    - https://developer.mozilla.org/en-US/docs/Learn/Forms/Sending_forms_through_JavaScript
    - https://developer.mozilla.org/en-US/docs/Web/API/Fetch_API
  - Add a screenshot button.  three.js has a tutorial for this.
  - Represent Assns<> between objects somehow.  Maybe highlight parents and children a different color?  Turns out we're not creating any Assns<> on EMPHATIC anyway.
  - Use Object3D.userData to display a window of information about hovered objects.  The magnet could display the magnetic field, or the SSDs could show a 2D view.

## Helpful links
- GUI example: https://github.com/georgealways/lil-gui
- three.js manual: https://threejs.org/manual/#en/fundamentals
