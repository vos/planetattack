// choose own planet at random
var myPlanets = Player.getPlanets();
if (myPlanets.length > 0) {
    var myPlanet = myPlanets[Random.randomInt(0, myPlanets.length)];
    // choose random target planet
    var otherPlanets = Player.getOtherPlanets();
    if (otherPlanets.length > 0) {
        var otherPlanet = otherPlanets[Random.randomInt(0, otherPlanets.length)];
        // attack other planet with 25-75% of my choosen planets resources
        var resourceFactor = Random.randomReal(0.25, 0.75);
        Player.resourceFactor = resourceFactor;
        Player.addShip(myPlanet, otherPlanet);
    } else {
        // no other planets found!
    }
} else {
    // I have no planets! :o
}

// set a new delay until the next attack
Player.AI.delay = Random.randomInt(250, 1000);
