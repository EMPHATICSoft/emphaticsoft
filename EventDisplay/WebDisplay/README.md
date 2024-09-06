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

- The event display just draws LineSegments and MC trajectories right now.  No tracks, hits, or Cherenkov information (yet!).  The magnet and target positions are from the Geometry service.  The magnet geometry is the only geometry component loaded directly from the GDML file right now.  Ask if you want more volumes like this.  I'm trying to limit detailed geometry to keep the focus on the physics.
- The WebDisplay is acting as a trivial "web server" that sends and receives HTTP over a port.  It now figures out when to go to the next event by counting requests.  I get 1 request per magnet segment + 1 request to go to the new event.  You'll get a bunch of numbers on your screen if it gets out of sync somehow.  Please tell me if that happens so I can fix it!
- You have to get port 3490 from the ART job to your web browser on your device.  In these instructions, it is forwarded by ssh which should even encrypt traffic over the port and requires you to authenticate using kerberos to use it effectively.  Not so friendly to non-collaborators, but hopefully overly safe since I'm a web neophyte.
- The 3D display is run client-side (on your computer, not the GPVM) using a Javascript library called three.js.  I got the idea to use this library from CMS's outreach event display.
- The GPVM "web server" is just a code generator that fills in some Javascript based on the LineSegments in the display and the Geometry service.
- We require a POSIX OS (i.e. not Windows) for 2 reasons that I know of: the sockets API, and the stat() API.  stat() is easy to replace with ROOT shenanigans that I don't like from TSystem.  ROOT and Boost may have more robust socket implementations.  The only reason I didn't use a portable socket implementation was to save myself work on the demonstration.

## TODOs
- Before next release:
  - Make sure render() happens when loading the next event is complete.  Can I also make sure it doesn't happen before loading is complete?  I think I'll have to put all Object3Ds from the next event into my own array, then call scene.clear() and add() them after all objects are loaded.  Go back to the old version of the geometry loading procedure because it doesn't fail and is less confusing.
  - Random access to events
  - Why doesn't this work on Safari?  Is it some security setting?  Should we disable the setting or change how the application works?
- Before next collaboration meeting:
  - Include SSD Tracks and TrackSegments.  Robert has these working now!
  - Include the ARICH.  Draw boxes for the ARICH hits?  Paste the 2D histogram on the front of the box as a texture?
  - How do we manage port numbers with many users on the same GPVM?  Right now, they see each other's event displays!
  - Better default camera positions and buttons to switch between them!
  - Better camera controls?  What do collaborators think of these OrbitControls?  Would others like arrow key navigation, or is the camera good enough as is?
  - Do we have reconstructed SSD tracks?
  - Add a list tree GUI?  It might help the MC make more sense, but I've always found them to be clutter to the majority of use cases.
  - Why are the downstream SSDs in weird positions?  Is something wrong with their units again, or is there a problem on the Javascript side?
- Before next run:
- Later...
  - Extensibility: Goal is for ordinary analyzers to add information to display without writing graphics code.  Better if they don't have to modify event display code at all.
    - Medium difficulty: evd::Metadata data products associated with any data product the event display consumes.  Has color and name overrides.
    - Harder difficulty: ART Tools that can send arbitrary cubes that frontend knows how to render.
  - Display context about selected objects.  We could query the ART job like it's a database!  The backend just has to send back HTML.  Some awesome things I want to do:
    - Show ART provenance information for any data product.  Maybe as a context menu?  How do I even do context menus in THREE.js?
    - Show histograms of the SSDs when their volumes are selected
    - Talk about what magnetic field, alignment constants, and beam conditions are loaded.  Selecting geometry objects is an opportunity to make "ergonomic" controls!
  - Port this to a real web server running e.g. at Notre Dame?  That would be nice for outreach!  But then someone has to understand and maintain a web server application.  The webserver I'm imagining would produce a web page that looks like Nathaniel Tagg's Arachne event display for MINERvA.  We could make an ART job act like a "CGI script" by reading requests on STDIN and replying on STDOUT.  It even sounds like we could leave the ART job running between serving events with Apache.  When I searched for ideas using Google, I saw recommendations that we might as well write an Apache module if we're forced to use C.  I'm not sure how to do that, and it sounds like more dependencies to me.
  - For the SSDs and other aligned components, we should at least apply alignment constants somehow.  The event display doesn't know about rotations to any objects right now because I need to convert from polar coordinates to Euler angles for THREE.js.
  - Persist camera settings across events.  I could either cache them somehow or do a massive redesign of the Javascript so that I only send positions and rotations for each event but keep the rendering code.  The latter probably makes us more robust in the long run, but that's a lot of work!  And a lot of things I don't yet know how to do in Javascript.
  - How often do we have to update the Javascript side of this application?  How often does the THREE.js API change?  Not work with certain browsers?  As a Javascript newcomer, I don't have easy answers to these questions.  CMS made it work somehow, so I have some hope this won't be too bad...
  - Random access to events.  I might end up sending and parsing web forms.  Some good resources to get me started:
    - https://developer.mozilla.org/en-US/docs/Web/HTTP/Methods
    - https://developer.mozilla.org/en-US/docs/Learn/Forms/Sending_forms_through_JavaScript
    - https://developer.mozilla.org/en-US/docs/Web/API/Fetch_API
  - Add a screenshot button.  three.js has a tutorial for this.
  - Represent Assns<> between objects somehow.  Maybe highlight parents and children a different color?  Turns out we're not creating any Assns<> on EMPHATIC anyway.
  - Use Object3D.userData to display a window of information about hovered objects.  The magnet could display the magnetic field, or the SSDs could show a 2D view.
  - Do we want any other volumes rendered directly from the GDML?  It should "just work" with a few changes to the magnet loading code.
  - Use/write a real web server/framework.  Then I wouldn't have to load magnet components 1 at a time and in order.  A real web framework probably has threads or forked processes that do this.  Then again, we're not exactly suffering from the latency.

## Helpful links
- GUI example: https://github.com/georgealways/lil-gui
- three.js manual: https://threejs.org/manual/#en/fundamentals
- Wavefront file format that I'm using TGeoShapes: https://en.wikipedia.org/wiki/Wavefront_.obj_file

## Version 2?
So far, the web-based event display client-side code, the front end, is rewritten by a code generator for each event.  I want to refactor it into a standalone front end and back end.  The front end will be a fuly functional Javascrpt program embedded in an HTML file.  The backend will send information about objects to draw by responding to GET calls.  The back end will also know to switch to a new event when responding to a POST call.

### Advantages
- Easier to debug the front end
- Can change out back end for a real web server more easily
- Easier to share front end with other small experiments

### Implementation Plan
- Use something like fetch() to request files.  That seems to be what THREE.OBJLoader is doing.
- Geometry transported as .obj files.  Physics objects transported as something like JSON or XML files.  I prefer JSON over a .txt or .obj file so I can embed metadata like particle name and energy
- Back end generates data files as the front end requests them
- Back end needs functions to parse GET and POST requests
- TODO: How does client "discover" what geometry volumes to request?  Same for what data types are available.
  - GET geometry/index.html to figure out list of geometry to draw?
  - try-catch around fetch()es that can fail to determine what physics data is available
  - **Instead**, use browser reply to POST request for new event.  Include URLs to request in response body.
