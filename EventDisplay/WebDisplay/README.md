#EMPHATIC Web Display
A browser-based event display for the EMPHATIC experiment.

Where I am so far:
- I have an ART module that acts like a web server... sometimes.  Not sure why it resets the connection other times.
- I can forward port 3490 from the GPVM (inside a container no less) to my laptop so my web browser can see it as localhost:3490
- I have a 3D viewer for boxes set up.  I now have some realistic SSD-like boxes to draw.  But they're not easy to view...
  - The 3D viewer is small compared to the scale of SSD strips!  I need to make the camera "bigger".
  - The camera can only see 2 LineSegments near the origin.  How do I extend the back clipping plane so it can see farther?  Increasing the distance of this plane doesn't seem to help.
  - The other SSD digits may be in shadow
  - Switching to an FPS-like camera might be convenient for debugging.  I have the orbit camera from a tutorial right now.
- My next step is further debugging the 3D viewer locally before I send it back to the web server.  See /media/evoPlus2TB/app/emphatic/webDisplay/deleteMe for my latest version
- A button to request the next event would be nice once I get the 3D viewer working
