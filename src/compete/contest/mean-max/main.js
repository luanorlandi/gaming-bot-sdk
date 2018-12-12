const PLAYERS = 3;
const MAP_RADIUS = 6000;
const MAP_MAX_DISTANCE = 2 * MAP_RADIUS;
const MY_ID = 0;

const SKILL = 'SKILL';
const WAIT = 'WAIT';
const MAX_THROTTLE = 300;
const RAGE_CAP = 300;

const REAPER = 0;
const DESTROYER = 1;
const DOOF = 2;
const TANKER = 3;
const WRECK = 4;
const TAR_POOL = 5;
const OIL_POOL = 6;

const TAR_RANGE = 2000;
const TAR_RADIUS = 1000;
const TAR_DURATION = 3;
const TAR_COST = 30;

const GRENADE_RANGE = 2000;
const GRENADE_RADIUS = 1000;
const GRENADE_DURATION = 1;
const GRENADE_COST = 60;

const OIL_RANGE = 2000;
const OIL_RADIUS = 1000;
const OIL_DURATION = 3;
const OIL_COST = 30;

class Game {
    constructor() {
        this.unitCount;
        this.players = [];
        this.units = [];
        this.reapers = [];
        this.doofs = [];
        this.destroyers = [];
        this.tankers = [];
        this.wrecks = [];
        this.tarPools = [];
        this.oilPools = [];
    }

    newPlayers() {
        this.players.length = 0;

        this.players.push(new Player(0));
        this.players.push(new Player(1));
        this.players.push(new Player(2));
    }

    newUnit(id, type, playerId) {
        if(type == REAPER) {

            const reaper = new Reaper();
            this.players[playerId].reaper = reaper;
            this.reapers.push(reaper);
            return reaper;

        } else if(type == DESTROYER) {

            const destroyer = new Destroyer();
            this.players[playerId].destroyer = destroyer;
            this.destroyers.push(destroyer);
            return destroyer;

        } else if(type == DOOF) {

            const doof = new Doof();
            this.players[playerId].doof = doof;
            this.doofs.push(doof);
            return doof;

        } else if(type == TANKER) {

            const tanker = new Tanker();
            this.tankers.push(tanker);
            return tanker;

        } else if(type == WRECK) {

            const wreck = new Wreck();
            this.wrecks.push(wreck);
            return wreck;

        } else if(type == TAR_POOL) {
            
            const tarPool = new TarPool();
            this.tarPools.push(tarPool);
            return tarPool;

        } else if(type == OIL_POOL) {
            
            const oilPool = new OilPool();
            this.oilPools.push(oilPool);
            return oilPool;

        } else {
            return new Unit();
        }
    }

    readUnit() {
        const inputs = readline().split(' ');
        const id = parseInt(inputs[0]);
        const type = parseInt(inputs[1]);
        const playerId = parseInt(inputs[2]);
        const mass = parseFloat(inputs[3]);
        const radius = parseInt(inputs[4]);
        const x = parseInt(inputs[5]);
        const y = parseInt(inputs[6]);
        const vx = parseInt(inputs[7]);
        const vy = parseInt(inputs[8]);
        const extra = parseInt(inputs[9]);
        const extra2 = parseInt(inputs[10]);

        const unit = this.newUnit(id, type, playerId);
        this.units.push(unit);

        unit.id = id;
        unit.type = type;
        unit.playerId = playerId;
        unit.mass = mass;
        unit.radius = radius;
        unit.x = x;
        unit.y = y;
        unit.vx = vx;
        unit.vy = vy;
        unit.extra = extra;
        unit.extra2 = extra2;
    }

    updatePlayers() {
        this.newPlayers();

        this.players[0].score = parseInt(readline());
        this.players[1].score = parseInt(readline());
        this.players[2].score = parseInt(readline());
        this.players[0].rage = parseInt(readline());
        this.players[1].rage = parseInt(readline());
        this.players[2].rage = parseInt(readline());
    }

    updateUnits() {
        this.units.length = 0;
        this.reapers.length = 0;
        this.doofs.length = 0;
        this.destroyers.length = 0;
        this.tankers.length = 0;
        this.wrecks.length = 0;
        this.tarPools.length = 0;
        this.oilPools.length = 0;

        this.unitCount = parseInt(readline());

        for(let i = 0; i < this.unitCount; i++) {
            this.readUnit();
        }
    }

    update() {
        this.updatePlayers();
        this.updateUnits();
    }
    
    byHighestScore(unitA, unitB) {
        const playerA = this.players[unitA.playerId];
        const playerB = this.players[unitB.playerId];

        return playerB.score - playerA.score;
    }
    
    enemyUnit(unit) {
        return unit.playerId != MY_ID;
    }

    getWinningEnemyReapers() {
        const reapers = this.reapers.sort(this.byHighestScore.bind(this));
        return reapers.filter(this.enemyUnit);
    }
}

const game = new Game();

class Player {
    constructor(id) {
        this.id = id;
        this.score;
        this.rage;
        this.reaper;
        this.destroyer;
        this.doof;

        this.enemyReapers;
    }

    decideReaperMove() {
        if(game.wrecks.length > 0) {
            this.reaper.searchTargetImproved(game.wrecks);
        } 
        
        if(!this.reaper.target) {
            this.reaper.target = this.destroyer;
            this.reaper.targetDistance = this.reaper.calculateDistance(this.destroyer);
        }

        this.reaper.steerToTarget();
    }

    decideDestroyerMove() {
        this.destroyer.searchTarget(game.tankers);

        if(this.destroyer.target) {
            const TOLERANCE = 800;
            const radius = this.destroyer.radius + this.destroyer.target.radius;
            const speed = this.destroyer.calculateSpeed();
            const closeDistance = TOLERANCE + radius + speed;

            const empty = this.destroyer.target.extra <= 1;

            if(this.destroyer.targetDistance < closeDistance) {
                const reapers = this.destroyer.target.getNearSorted(game.reapers);

                if(empty || reapers[0].playerId != this.id) {
                    this.destroyer.steerAwayOfTarget();
                    return;
                }
            }
        } else {
            this.destroyer.steerToCenter();
            return;
        }
        
        this.destroyer.steerToTarget();
    }

    decideDoofMove() {
        const reaper = this.enemyReapers[0];

        if(this.rage >= this.doof.skill.cost) {
            this.doof.searchTargetSkill(reaper);

            if(this.doof.target) {
                this.doof.skillToTarget();
                this.rage -= this.doof.skill.cost;
                return;
            }
        }
        
        this.doof.target = reaper;
        this.doof.moveToTarget(300);
    }

    decideMoves() {
        this.enemyReapers = game.getWinningEnemyReapers();
        this.enemyReapers.forEach(function(reaper) {
            reaper.searchTarget(game.wrecks);
        });

        this.decideReaperMove();
        this.decideDestroyerMove();
        this.decideDoofMove();
    }
}

class Skill {
    constructor(range, radius, duration, cost) {
        this.range = range;
        this.radius = radius;
        this.duration = duration;
        this.cost = cost;
    }
}

class Unit {
    constructor() {
        this.id;
        this.type;
        this.playerId;
        this.mass;
        this.radius;
        this.x;
        this.y;
        this.vx;
        this.vy;
        this.extra;
        this.extra2;

        this.friction;
        this.skill;

        this.target;
        this.targetDistance;
    }

    moveTo(x, y, throttle, message) {
        print(Math.round(x) + ' ' + Math.round(y) + ' ' + Math.round(throttle) + ' ' + message);
    }

    skillTo(x, y, message) {
        print(SKILL + ' ' + Math.round(x) + ' ' + Math.round(y) + ' ' + ' ' + message);
    }

    wait(message) {
        print(WAIT + ' ' + message);
    }

    isOutsideMap() {
        const distance = Math.sqrt(this.x * this.x + this.y * this.y);
        
        return distance > MAP_RADIUS;
    }
    
    isOutrange(unit) {
        return this.calculateDistance(unit) > this.skill.range;
    }

    calculateDistance(unit) {
        const x = this.x - unit.x;
        const y = this.y - unit.y;
        return Math.sqrt(x * x + y * y);
    }

    calculateSpeed() {
        return Math.sqrt(this.vx * this.vx + this.vy * this.vy);
    }

    closestTarget(closest, unit) {
        if(unit.isOutsideMap && unit.isOutsideMap()) {
            return closest;
        }

        if(!closest) {
            return {
                target: unit,
                targetDistance: this.calculateDistance(unit)
            };
        }

        const distance = this.calculateDistance(unit);

        if(distance < closest.targetDistance) {
            return {
                target: unit,
                targetDistance: distance
            };
        } else {
            return closest;
        }
    }
    
    closestTargetInRange(closest, unit) {
        if(unit.isOutside && unit.isOutside()) {
            return closest;
        }

        if(this.isOutrange(unit)) {
            return closest;
        }

        if(!closest) {
            return {
                target: unit,
                targetDistance: this.calculateDistance(unit)
            };
        }

        const distance = this.calculateDistance(unit);

        if(distance < closest.targetDistance) {
            return {
                target: unit,
                targetDistance: distance
            };
        } else {
            return closest;
        }
    }
    
    closestUnit(unitA, unitB) {
        const distanceA = this.calculateDistance(unitA);
        const distanceB = this.calculateDistance(unitB);

        return distanceA - distanceB;
    }
    
    closestUnitHeuristic(unitA, unitB) {
        const distanceA = this.calculateDistance(unitA);
        const distanceB = this.calculateDistance(unitB);

        const waterA = unitA.extra;
        const waterB = unitB.extra;

        const valueA = distanceA - 20 * Math.pow(waterA, 2);
        const valueB = distanceB - 20 * Math.pow(waterB, 2);

        return valueA - valueB;
    }
    
    getNearSorted(units) {
        return units.sort(this.closestUnit.bind(this));
    }
    
    getNearSortedHeuristic(units) {
        return units.sort(this.closestUnitHeuristic.bind(this));
    }
    
    searchTarget(units) {
        const closest = units.reduce(this.closestTarget.bind(this), null);

        if(closest) {
            this.target = closest.target;
            this.targetDistance = closest.targetDistance;
        }
    }
    
    searchTargetImproved(units) {
        const unitsSorted = this.getNearSortedHeuristic(units);

        const closest = unitsSorted.find(function(unit) {
            if(unit.isOnOilPool()) {
                return false;
            }

            const TOO_FAR = MAP_RADIUS;

            if(unit.extra <= 1 && this.calculateDistance(unit) > TOO_FAR) {
                return false;
            }

            return true;
        }.bind(this));

        if(closest) {
            this.target = closest;
            this.targetDistance = this.calculateDistance(closest);
        }
    }
    
    searchTargetInRange(units) {
        const closest = units.reduce(this.closestTargetInRange.bind(this), null);

        if(closest) {
            this.target = closest.target;
            this.targetDistance = closest.targetDistance;
        }
    }

    moveToTarget(throttle) {
        if(!this.target) {
            this.wait('no target to move');
            return;
        }

        const x = this.target.x;
        const y = this.target.y;

        this.moveTo(x, y, throttle, 'move to ' + this.target.id);
    }

    steerToTarget() {
        if(!this.target) {
            this.wait('wait');
            return;
        }

        const x = this.target.x - this.vx;
        const y = this.target.y - this.vy;

        this.moveTo(x, y, MAX_THROTTLE, 'steer to ' + this.target.id);
    }
    
    steerAwayOfTarget() {
        if(!this.target) {
            this.wait('wait');
            return;
        }

        const x = this.vx - this.target.x;
        const y = this.vy - this.target.y;

        this.moveTo(x, y, MAX_THROTTLE, 'away of ' + this.target.id);
    }
    
    skillToTarget() {
        if(!this.target) {
            this.wait('no target to skill');
            return;
        }

        const x = this.target.x;
        const y = this.target.y;

        this.skillTo(x, y, 'skill to ' + this.target.id);
    }
}

class Reaper extends Unit {
    constructor() {
        super();

        this.friction = 0.2;
        this.skill = new Skill(TAR_RANGE, TAR_RADIUS, TAR_DURATION, TAR_COST);
    }
}

class Destroyer extends Unit {
    constructor() {
        super();

        this.friction = 0.3;
        this.skill = new Skill(GRENADE_RANGE, GRENADE_RADIUS, GRENADE_DURATION, GRENADE_COST);
    }

    steerToCenter() {
        const x = 0 - this.vx;
        const y = 0 - this.vy;

        const throttle = MAX_THROTTLE/2;

        this.moveTo(x, y, MAX_THROTTLE/2, 'center with ' + throttle);
    }

}

class Doof extends Unit {
    constructor() {
        super();

        this.friction = 0.25;
        this.skill = new Skill(OIL_RANGE, OIL_RADIUS, OIL_DURATION, OIL_COST);
    }

    searchTargetSkill(reaper) {
        if(!reaper.target) {
            return;
        }

        const isTargetFar = this.calculateDistance(reaper.target) > this.skill.range;

        if(isTargetFar) {
            return;
        }

        const speed = reaper.calculateSpeed();
        const isReaperFarOfTarget = reaper.targetDistance - speed > reaper.target.radius;

        if(isReaperFarOfTarget) {
            return;
        }
        
        const isOnOilPool = reaper.target.isOnOilPool();

        if(isOnOilPool) {
            return;
        }

        this.target = reaper.target;
    }
}

class Tanker extends Unit {
    constructor() {
        super();

        this.friction = 0.4;
    }
}

class Wreck extends Unit {
    constructor() {
        super();
    }

    isCloseToPool(pool) {
        if(pool.extra === 0) {
            return false;
        }

        const distance = this.calculateDistance(pool);

        return distance + this.radius <= pool.radius;
    }

    isOnOilPool() {
        if(game.oilPools.length === 0) {
            return false;
        }

        return game.oilPools.some(this.isCloseToPool.bind(this));
    }
}

class TarPool extends Unit {
    constructor() {
        super();
    }
}

class OilPool extends Unit {
    constructor() {
        super();
    }
}

while(true) {
    game.update();
    game.players[MY_ID].decideMoves();
}