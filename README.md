# Hex defense

Welcome to my **Hex Tower Defense** game, the goal is to learn cpp (my goal).
Your goal in this game is to survive the swarm of hex characters, crawling to your pc.
The hex characters are actually a mallicious packets that try to break into your computer.
Throughout the game you devise a plan to stop the attacker and even get your **revenge**.

# How to play    
## Edit level
In main menu, by pressing 'e', you can choose witch level you want to edit by pressing nuber from zero to six (0-6).

- Your curosor is '@' and you can move using arrows.
- By pressing any printable character except from space you will select it.
- By pressing space you print the selected character.
- By pressing Enter you toggle automatic printing.
- By pressing 'q' you save the level.

Path for enemies consist of starting point '*' and ending point '#', which are connected by '.'.
To create a playable level it has to contain one start, one finish and all of the tiles of the road '.' have to have only two tiles as neighbours (including start and end). 

## Play level
In main menu, by pressing 'p', you can choose witch level you want to play by pressing nuber from zero to six (0-6).
You can choose the difficulty by pressng number from one to three (1-3).

- Your curosor is '@' and you can move using arrows.
- By pressing [jkl] you can choose tower, by pressing Enter you place it.
- 'h' switches you back to cursor mode.
- 'u' while cursor is on tower upgrades it.
- 'd' while cursor is on tower sells it.
- 's' starts new round.
- 'f' changes speed of the game.
- 'q' quits the game.


# Dependency

Only dependency for this project is libncurses-dev.

# Current state
The game is far from finished, but will stay in it's current form for at least few months.
- The game does not contain any story.
- Enemy movement calculation is very inefficient.
- Tutorial is missing
- Documentation is missing.
- Only three towers atm.

# Sources

**Ncurses tutorial series**:
https://www.youtube.com/watch?v=A5lX1h_2zy0&list=PL2U2TQ__OrQ8jTf0_noNKtHMuYlyxQl4v&index=2

**ASCII title**:
http://patorjk.com/software/taag/#p=display&f=Doom&t=Hex%20defense

