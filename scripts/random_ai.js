// choose own planet at random
if (Player.planetCount > 0) {
    var myPlanet = Player.getRandomPlanet();
    // choose random target planet
    var otherPlanets = Player.getOtherPlanets();
    if (otherPlanets.length > 0) {
        var otherPlanet = otherPlanets[Random.randomInt(0, otherPlanets.length)];
        // attack other planet with 25-75% of my choosen planets resources
        var resourceFactor = Random.randomReal(0.25, 0.75);
        Player.resourceFactor = resourceFactor;
        myPlanet.transferResourcesTo(otherPlanet);
    }
}

// set a new delay until the next attack
Player.AI.delay = Random.randomInt(250, 1000);
