# A-Star

#### A\* implementation.
Some great information about A\* can be found [here](http://theory.stanford.edu/%7Eamitp/GameProgramming/AStarComparison.html) by [Amir Patel](https://www.redblobgames.com/).  
Wikipedia also has a decent introduction to the topic: [A\* search algorithm](https://en.wikipedia.org/wiki/A*_search_algorithm).  

Code is easily modifiable if you want to implement different kinds of walkable terrains (water, sand, bridges etc.), you just need to modify the `movement_cost` function.  

Also, all the examples are done by allowing 4-directional movement. In order to allow movement in other directions, modify `find_neigbours` function.

#### Usage
```
Usage: a-star [OPTIONS] [FILE]

OPTIONS:
    --help - print this message and exit
```

#### Map examples can be found in `/examples` directory.

#### Symbols
* `S` starting point
* `G` goal point
* `.` unexplored node
* `~` explored node
* `#` obstacle
* `+` path
