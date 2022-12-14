Project
-------

The project implements the Hex game. The spirit is to understand and use most of the C++11 and onwards features
by writing a non-trivial program.

[Hex game](https://en.wikipedia.org/wiki/Hex_(board_game))
[Play Hex online](http://www.lutanho.net/play/hex.html)

The AI uses a Montecarlo simulation to compute the next move while a graph implements the Hex board, some level
of parallelism is achieved to speed up the trials using the asynch-task/future functionalities provided by the
newer releases of C++.

Other AI algorithms touched during the course are the min-max and alpha-beta.

This is the main homework - developed in stages - for the Coursera course
`https://www.coursera.org/learn/c-plus-plus-b/home/week/4`

Building
-------

The project it's straightforwared to compile, just issue to generate an executable a.out:

`g++ HexBoard.cpp -o hex -lpthread`

Running
-------

The executable asks the end user a few questions before starting the game.
