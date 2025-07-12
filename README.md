This is a small game project to solidify and test some netcode stuff before I
start implementing netcode on my larger game project (prototype 1 in my MEMO67
engine).


Initial idea as of 20250705 is:

  - Naive TCP login.
  - UDP game connections.
  - Slitherio-like game where square players eat world-spawned food.
  - Server will handle world state, players can be authoritative over their
  position and attempts to eat food.
  
20250711 - v.0

  As of 20250711 this project has the features outlined above, in a minimal
  functional state. This is mostly just test code and has served its educational
  purpose. Robustness should be addressed, multithreading resource concurrency
  stuff needs handling, packets need to properly handle endianness, there are a
  few known bugs, and overall structure could use a good consolidation and
  cleanup pass. I've added a license but I wouldn't recommend using this project
  until the above points are addressed (at minimum).
  
![screenshot](img.jpg)

#####
License
======
Licensed under GPL3.0, see [LICENSE.txt](LICENSE.txt)

While the license permits broad use, redistribution, and modification, the
author requests that this software is NOT used, in whole or in part, for
training, tuning, evaluating, or developing any machine learning, artificial
intelligence, or generative systems. This request does not apply to finite state
machines, behaviour trees, or similar deterministic systems commonly referred to
as artificial intelligence within the context of video games, robotics, and
other non-learning applications.
