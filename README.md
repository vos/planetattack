# Planet Attack

Space RTS game.

--- pre alpha ---


## AI Scripting API Reference

### Global Objects

- **Random** : *RandomUtil*
 - **Random.randomInt**() : *int*  -- between 0 and inf*
 - **Random.randomInt**(from : *int*, to : *int*) : *int*  -- inclusive from and exclusive to
 - **Random.randomReal**() : *float*  -- between 0.0 and 1.0
 - **Random.randomReal**(min : *float*, max : *float*) : *float*

- **Game** : *Canvas*
  - **Game.playerCount** : *int* [readonly]
  - **Game.getPlayers**() : *Array[Player]*
  - **Game.getRandomPlayer**() : *Player*
  - **Game.planetCount** : *int* [readonly]
  - **Game.getPlanets**() : *Array[Planet]*
  - **Game.addPlanet**(planet : *Planet*)
  - **Game.addPlanet**(position : *Vector2*[, radius : *float*, resources : *float*]) : *Planet*
  - **Game.addPlanet**(xpos : *float*, ypos : *float*[, radius : *float*, resources : *float*]) : *Planet*
  - **Game.removePlanet**(planet : *Planet*)
  - **Game.getRandomPlanet**() : *Planet*

- **Game.Time** : *GameTime*
  - **Game.Time.total** : *int*  -- milliseconds since the game start
  - **Game.Time.elapsed** : *int*  -- milliseconds since the last update (Frame)
  - **Game.Time.elapsedSeconds** : *float*  -- seconds since the last update (Frame)

### AI Member Objects

- **Player** : *Player*
- **Player.AI** : *PlayerIntelligence*
  - **Player.AI.delay** : *float*  -- delay of script execution in milliseconds

### Introduced Types

#### Vector2

- **Properties**
  - **x** : *float* [readonly\*]
  - **y** : *float* [readonly\*]

#### Color : *string*

See [QColor](http://qt-project.org/doc/qt-4.8/qcolor.html#setNamedColor) named colors.

#### Player

- **Properties**
  - **name** : *string*
  - **color** : *Color*
  - **resourceFactor** : *float*
  - **human** : *bool* [readonly]
  - **computer** : *bool* [readonly]  -- AI
  - **planetCount** : *int* [readonly]
  - **shipCount** : *int* [readonly]

- **Functions**
  - **getPlanets**() : *Array[Planet]*
  - **addPlanet**(planet : *Planet*)
  - **addPlanet**(position : *Vector2*[, radius : *float*, resources : *float*]) : *Planet*
  - **addPlanet**(xpos : *float*, ypos : *float*[, radius : *float*, resources : *float*]) : *Planet*
  - **removePlanet**(planet : *Planet*)
  - **getRandomPlanet**() : *Planet*
  - **getShips**() : *Array[Ship]*
  - **addShip**(ship : *Ship*)
  - **removeShip**(ship : *Ship*)
  - **getRandomShip**() : *Ship*
  - **getEnemies**() : *Array[Player]*
  - **getRandomEnemy**() : *Player*
  - **getEnemyPlanets**() : *Array[Planet]*
  - **getRandomEnemyPlanet**() : *Planet*
  - **getOtherPlanets**() : *Array[Planet]*  -- enemy and neutral planets
  - **getRandomOtherPlanet**() : *Planet*

- **Signals**
  - **nameChanged**(oldName : *string*, newName : *string*)

#### Planet

- **Properties**
  - **position** : *Vector2*
  - **resources** : *float*
  - **color** : *Color*
  - **player** : *Player*
  - **radius** : *float*
  - **productionFactor** : *float*
  - **neutral** : *bool* [readonly]

- **Functions**
  - **transferResourcesTo**(target : *Planet*[, resourceFactor: *float*]) : *Ship*

#### Ship

- **Properties**
  - **position** : *Vector2*
  - **resources** : *float*
  - **color** : *Color*
  - **player** : *Player*
  - **target** : *Player*
  - **speed** : *float*  -- pixels per second
