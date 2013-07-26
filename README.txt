Siftkey: a sifteo smart key proof of concept.

Scenario
========
In this project, we implement an internet of thing concept to simplify everyday task management.
There are 2 types of cubes, the keys and the bolts.
Each key cube represents a uset and each bolt represents a task.
The keys are used to unlock tasks and score points.

Details
=======
This code allows listening to sensor events and logs them to a csv file using this instruction:

  swiss listen siftkey.elf > ~/Dropbox/siftkey/log.csv

The csv file is synchronised with another computer using dropbox.
A javascript program parses it and uses d3js.org to visualise the data.

Usage
=====
You need the sifteo SDK to use this code:

  www.sifteo.com/developers

Enjoy !
Leslie & Drix.

