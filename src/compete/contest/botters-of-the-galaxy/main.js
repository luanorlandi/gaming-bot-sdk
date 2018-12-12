// map size
const MAP_WIDTH = 1920;
const MAP_HEIGHT = 750;

// entity type
const TYPE_MINION = 'UNIT';
const TYPE_TOWER = 'TOWER';
const TYPE_GROOT = 'GROOT';
const TYPE_HERO = 'HERO';
const TYPE_BUSH = 'BUSH';
const TYPE_SPAWN = 'SPAWN';

// hero
const DEADPOOL = 'DEADPOOL';
const DOCTOR_STRANGE = 'DOCTOR_STRANGE';
const HULK = 'HULK';
const IRONMAN = 'IRONMAN';
const VALKYRIE = 'VALKYRIE';

// commands for hero
const MOVE = 'MOVE';
const ATTACK = 'ATTACK';
const ATTACK_NEAREST = 'ATTACK_NEAREST';
const MOVE_ATTACK = 'MOVE_ATTACK';
const WAIT = 'WAIT';

// hero status
const STATUS_AGGRESSIVE = 'AGGRESSIVE';
const STATUS_RETREAT = 'RETREAT';
const STATUS_FARMING = 'FARMING';

// game logic constants
const MIN_DENY_MULTIPLIER = 0.4;
const AGGRO_RANGE = 300;

// my constants
const DISTANCE_CLOSE = 220;
const MINION_ATTACK_TOLERANCE = 15;

class Point {
  constructor(x, y) {
    this.state = {x, y};
  }

  distanceTo(point) {
    const x = this.state.x - point.state.x;
    const y = this.state.y - point.state.y;

    return Math.sqrt(x * x + y * y);
  }

  closestPosition(range, point) {
    var distance = this.distanceTo(point);
    // printErr('pointTowards this: ', this.state.x, this.state.y);
    // printErr('pointTowards point: ', point.state.x, point.state.y);
    // printErr('pointTowards range: ', range);
    // printErr('pointTowards distance: ', distance);
    if (distance > range) {
      // const distanceX = Math.abs(point.state.x - this.state.x) * range / distance;
      // const distanceY = Math.abs(point.state.y - this.state.y) * range / distance;
      // printErr('pointTowards distanceXY: ', distanceX, distanceY);
      // const x = this.state.x > point.state.x ? this.state.x - distanceX : this.state.x + distanceX;
      // const y = this.state.y > point.state.y ? this.state.y - distanceY : this.state.y + distanceY;
      const x = (point.state.x - this.state.x) / distance * range + this.state.x;
      const y = (point.state.y - this.state.y) / distance * range + this.state.y;
      
    // printErr('RESULT POS: ', x, y);
    // printErr('RESULT DISTANCE: ', new Point(x, y).distanceTo(this));
    // printErr('RESULT DISTANCE: ', new Point(x, y).distanceTo(point));
    // printErr('RESULT DISTANCE SUM: ', new Point(x, y).distanceTo(this) + new Point(x, y).distanceTo(point));
      
      return new Point(x, y);
    }

    return this;
  }
  

  positionInDirection(range, point) {
    var distance = this.distanceTo(point);

    if (distance > 0.1) {
      const x = (point.state.x - this.state.x) / distance * range + this.state.x;
      const y = (point.state.y - this.state.y) / distance * range + this.state.y;

      return new Point(x, y);
    }

    return this;
  }

  setState(newState) {
    Object.assign(this.state, newState);
  }
}

class Unit {
  constructor() {
    this.state = {
      id: null,
      pos: null,
    };
  }

  distanceTo(unit) {
    if (unit.state.pos) {
      return this.state.pos.distanceTo(unit.state.pos);
    }
    
    // unit it's a point
    return this.state.pos.distanceTo(unit);
  }

  canDieNextTurn() {
    return this.state.health < 100;
  }
  

  // compareDistanceTo(a, b, target) {
  //   return
  // }

  unitsInRange(units, range, tolerance = 0) {
    return units.reduce((unitsInRange, unit) => {
      const distance = this.distanceTo(unit);

      if(distance <= range + tolerance) {
        return [...unitsInRange, unit];
      }

      return unitsInRange;
    }, []);
  }

  setState(newState) {
    Object.assign(this.state, newState);
  }
}

class Minion extends Unit {
  constructor() {
    super();
    const state = {
      team: null,
      type: null,
      attackRange: null,
      health: null,
      maxHealth: null,
      shield: null,
      attackDamage: null,
      movementSpeed: null,
      stunDuration: null,
      goldValue: null,
    };

    this.setState(state);
  }
}

class Tower extends Unit {
  constructor() {
    super();
    const state = {
      team: null,
      type: null,
      attackRange: null,
      health: null,
      maxHealth: null,
      shield: null,
      attackDamage: null,
      movementSpeed: null,
      stunDuration: null,
      goldValue: null,
    };

    this.setState(state);
  }

  isLeftSide() {
    return this.state.pos.state.x < MAP_WIDTH / 2;
  }

  getSafePosition() {
    if (this.isLeftSide()) {
      return new Point(this.state.pos.state.x - 100, this.state.pos.state.y);
    }

    return new Point(this.state.pos.state.x + 100, this.state.pos.state.y);
  }
}

class Groot extends Unit {
  constructor() {
    super();
    const state = {
      team: null,
      type: null,
      attackRange: null,
      health: null,
      maxHealth: null,
      shield: null,
      attackDamage: null,
      movementSpeed: null,
      stunDuration: null,
      goldValue: null,
    };

    this.setState(state);
  }
}

class Hero extends Unit {
  constructor() {
    super();
    const state = {
      team: null,
      type: null,
      attackRange: null,
      health: null,
      maxHealth: null,
      shield: null,
      attackDamage: null,
      movementSpeed: null,
      stunDuration: null,
      goldValue: null,
      countDown1: null,
      countDown2: null,
      countDown3: null,
      mana: null,
      maxMana: null,
      manaRegeneration: null,
      heroType: null,
      isVisible: null,
      itemsOwned: null,

      // new values
      status: null,
    };

    this.setState(state);
  }

  isVulnerable(allyMinions, allyTower) {
    return allyMinions.every(minion => {
      if (allyTower.isLeftSide()) {
        return this.state.pos.state.x > minion.state.pos.state.x;
      }
      
      return this.state.pos.state.x < minion.state.pos.state.x;
    });
  }

  moveToBase(allyTower, message) {
    const pos = new Point(null, allyTower.state.pos.state.y);
    if (allyTower.isLeftSide()) {
      pos.setState({x: allyTower.state.pos.state.x - 50});
    } else {
      pos.setState({x: allyTower.state.pos.state.x + 50});
    }
    
    this.move(pos, message);
  }

  farm(minions) {
    const target = minions.reduce((target, minion) => {
      const distance = this.distanceTo(minion);

      // check minion low health
      if (distance <= this.state.attackRange &&
        minion.state.health > 0 &&
        minion.state.health < this.state.attackDamage &&
        (target == null ||
        target.state.health > minion.state.health)) {

          return minion;
      }

      // check minion high health
      if (distance <= this.state.attackRange &&
        minion.state.health >= 2 * this.state.attackDamage &&
        (target == null ||
        target.state.health > minion.state.health)) {

          return minion;
      }

      return target;
    }, null);

    return target;
  }

  deny(minions) {
    const target = minions.reduce((target, minion) => {
      const distance = this.distanceTo(minion);

      // check minion low health
      if (distance <= this.state.attackRange &&
        minion.state.health > 0 &&
        minion.state.health < this.state.attackDamage &&
        minion.state.health < 0.4 * minion.state.maxHealth &&
        (target == null ||
        target.state.health > minion.state.health)) {

          return minion;
      }

      return target;
    }, null);

    return target;
  }

  findPosition(myTeam) {
    const MINIMUM_HEALTH = 100;
    const DISTANCE = this.state.attackRange;
    const myMinions = myTeam.state.minions;
    const myTower = myTeam.state.tower;

    // get farthest minion
    const target = myMinions.reduce((target, minion) => {
      const distanceTower = myTower.distanceTo(minion);
      minion.setState({distanceTower});

      if (minion.state.health > MINIMUM_HEALTH &&
        (target == null ||
        distanceTower > target.state.distanceTower)) {
        
        return minion;
      }

      return target;
    }, null);

    if (target == null) {
      return null;
    }

    if (myTower.isLeftSide()) {
      return new Point(target.state.pos.state.x - DISTANCE, target.state.pos.state.y);
    } else {
      return new Point(target.state.pos.state.x - DISTANCE, target.state.pos.state.y);
    }
  }

  findFarmPosition(myTeam, enemyTeam) {
    const myMinions = myTeam.state.minions;
    const enemyMinions = enemyTeam.state.minions;
    const myTower = myTeam.state.tower;

    // check if should go to the tower
    if (myMinions.length == 0) {
      // printErr('go to tower');
      return mytower.getSafePosition();
    }

    const myFarthestMinion = myMinions[0];
    const farthestEnemyMinion = enemyMinions[0];
    const range = this.state.attackRange;
    const gap = myTower.isLeftSide() ? -range : range;
    let pos;

    if (myFarthestMinion) printErr('myFarthestMinion', myFarthestMinion.state.id);
    if (farthestEnemyMinion) printErr('farthestEnemyMinion', farthestEnemyMinion.state.id);
    printErr('range', range);
    printErr('gap', gap);

    // check if there are enemy minions
    if (farthestEnemyMinion == null) {
      pos = myFarthestMinion.state.pos;
      printErr('no enemy minions', myFarthestMinion.state.id);
      return new Point(pos.state.x + gap, myTower.state.y);
    }

    const enemyRange = farthestEnemyMinion.state.attackRange;
    const unitsInRange = farthestEnemyMinion.unitsInRange(myMinions, enemyRange, MINION_ATTACK_TOLERANCE);

    printErr('unitsInRange', unitsInRange);
    if (unitsInRange) printErr('unitsInRange.length', unitsInRange.length);

    // check if there is at least 2 minions in front line
    if (unitsInRange.length >= 2) {
      pos = farthestEnemyMinion.state.pos;
      printErr('2 minions in frontline', farthestEnemyMinion.state.id);
      return new Point(pos.state.x + gap, pos.state.y);
    }

    // check if there is at least 1 minion with high health
    if (unitsInRange.length >= 1 && !unitsInRange[0].canDieNextTurn()) {
      pos = unitsInRange[0].state.pos;
      printErr('1 minion, high health', unitsInRange[0].state.id);
      return new Point(pos.state.x + gap, pos.state.y);
    }

    const safeUnit = myMinions[1] || myTower;
    pos = safeUnit.state.pos;
    printErr('safe unit', safeUnit.state.type);
    return new Point(pos.state.x + gap, pos.state.y);
  }

  move(pos, message = '') {
    print(MOVE + ' ' + pos.state.x + ' ' + pos.state.y + ';' + message);
  }
  
  attack(unit, message = '') {
    print(ATTACK + ' ' + unit.state.id + ';' + message);
    const health = unit.state.health - this.state.attackDamage;
    unit.setState({health});
  }

  attackNearest(unitType, message = '') {
    print(ATTACK_NEAREST + ' ' + unitType + ';' + message);
  }

  moveAttack(unit, pos, message = '') {
    if (pos) {
      print(MOVE_ATTACK + ' ' +
        pos.state.x + ' ' +
        pos.state.y + ' ' +
        unit.state.id + ';' +
        message);
      return;
    }

    print(MOVE_ATTACK + ' ' +
      unit.state.x + ' ' +
      unit.state.y + ' ' +
      unit.state.id + ';' +
      message);
  }

  wait(message = '') {
    print(WAIT + ';' + message);
  }
}

class Bush extends Unit {
  constructor() {
    super();
    const state = {
      radius: null,
    };

    this.setState(state);
  }
}

class Spawn extends Unit {
  constructor() {
    super();
    const state = {
      radius: null,
    };

    this.setState(state);
  }
}

class Item {
  constructor() {
    this.state = {
      itemName: null,
      itemCost: null,
      damage: null,
      health: null,
      maxHealth: null,
      mana: null,
      maxMana: null,
      moveSpeed: null,
      manaRegeneration: null,
      isPotion: null,
    };
  }

  setState(newState) {
    Object.assign(this.state, newState);
  }
}

class Team {
  constructor() {
    this.state = {
      gold: null,
      heroes: [],
      minions: [],
      tower: null,
    };
  }

  updateGold(gold) {
    this.setState({gold});
  }

  addHero(hero) {
    const heroes = this.state.heroes.concat(hero);
    this.setState({heroes});
  }

  addMinion(minion) {
    const minions = this.state.minions.concat(minion);
    this.setState({minions});
  }

  updateStatus(enemyTeam) {
    const myHeroes = this.state.heroes;
    const myMinions = this.state.minions;
    const enemyHeroes = enemyTeam.state.heroes;
    const enemyMinions = enemyTeam.state.minions;

    // heroes that are most close to the frontline
    const closestEnemyHero = this.heroClosestTo(enemyHeroes, myMinions[0].state.pos);
    const myClosestHero = this.heroClosestTo(myHeroes, enemyMinions[0].state.pos);

    // calculate which minions are in aggro range
    const myRange = myClosestHero.state.attackRange;
    const enemyRange = closestEnemyHero.state.attackRange;
    const myPosition = myClosestHero.state.pos;
    const enemyPosition = closestEnemyHero.state.pos;
    printErr('myClosestHero', myClosestHero.state.id);
    printErr('closestEnemyHero', closestEnemyHero.state.id);
    const myAttackPoint = enemyPosition.closestPosition(myRange, myClosestHero.state.pos);
    const enemyAttackPoint = myPosition.closestPosition(enemyRange, closestEnemyHero.state.pos);
    const myMinionsAggro = enemyTeam.minionsInAggroRange(myMinions, enemyAttackPoint);
    const enemyMinionsAggro = this.minionsInAggroRange(enemyMinions, myAttackPoint);
    let status;

    printErr('myMinions aggro length', myMinionsAggro.length);
    printErr('myMinions point attack', myAttackPoint.state.x, myAttackPoint.state.y);
    printErr('myMinions range', myRange);
    printErr('enemyMinions aggro length', enemyMinionsAggro.length);
    printErr('enemyMinions point attack', enemyAttackPoint.state.x, enemyAttackPoint.state.y);
    printErr('enemyMinions range', enemyRange);

    if (myMinionsAggro.length > enemyMinionsAggro.length) {
      status = STATUS_AGGRESSIVE;
    } else if(myMinionsAggro.length < enemyMinionsAggro.length) {
      status = STATUS_RETREAT;
    } else {
      status = STATUS_FARMING;
    }
    
    myHeroes.forEach((hero) => {
      hero.setState({status});
    });
  }

  heroLowestHealth(heroes) {
    return heroes.reduce((heroLowestHealth, hero) => {
      if (heroLowestHealth.state.health < hero.state.health) {
        return heroLowestHealth;
      }

      return hero;
    });
  }

  heroClosestTo(heroes, point) {
    return heroes.reduce((closestHero, hero) => {
      const distanceHero = hero.distanceTo(point);
      const distanceClosestHero = closestHero.distanceTo(point);

      if (distanceClosestHero < distanceHero) {
        return closestHero;
      }

      return hero;
    });
  }

  minionsInAggroRange(minions, point) {
    return minions.reduce((minionsInRange, minion) => {
      const distance = minion.distanceTo(point);

      if(distance <= AGGRO_RANGE) {
        printErr('minion in range: ', minion.state.id, distance);
        return [...minionsInRange, minion];
      }
      
      printErr('minion not in range: ', minion.state.id, distance);
      printErr('minion not in range: ', minion.state.pos.state.x);
      printErr('minion not in range: ', minion.state.pos.state.y);
      printErr('minion not in range: ', point.state.x);
      printErr('minion not in range: ', point.state.y);

      return minionsInRange;
    }, []);
  }

  clearUnits() {
    this.setState({
      heroes: [],
      minions: [],
    });
  }

  setState(newState) {
    Object.assign(this.state, newState);
  }
}

class Game {
  constructor() {
    this.state = {
      myTeam: null,
      enemyTeam: null,
      groots: [],
      team: [],
      bushes: [],
      spawns: [],
      items: [],
      roundType: null,
    };
  }

  updateMyTeam() {
    const myTeam = parseInt(readline());
    const enemyTeam = myTeam ? 0 : 1;
    this.setState({myTeam, enemyTeam});
  }

  addTeam() {
    const team = [...this.state.team, new Team()];
    this.setState({team});
  }

  addGroot(groot) {
    const groots = [...this.state.groots, groot];
    this.setState({groots});
  }

  addBush(bush) {
    const bushes = [...this.state.bushes, bush];
    this.setState({bushes});
  }

  addSpawn(spawn) {
    const spawns = [...this.state.spawns, spawn];
    this.setState({spawns});
  }

  addItem(item) {
    const items = [...this.state.items, item];
    this.setState({items});
  }

  updateGold() {
    const gold = parseInt(readline());
    const enemyGold = parseInt(readline());

    this.state.team[this.state.myTeam].updateGold(gold);
    this.state.team[this.state.enemyTeam].updateGold(enemyGold);
  }

  updateRoundType() {
    // a positive value will show the number of heroes that await a command
    const roundType = parseInt(readline());
    this.setState({roundType});
  }

  updateEntity() {
    const entityCount = parseInt(readline());
    for (let i = 0; i < entityCount; i++) {
      const inputs = readline().split(' ');

      const data = {
        id: parseInt(inputs[0]),
        team: parseInt(inputs[1]),
        type: inputs[2], // UNIT, HERO, TOWER, can also be GROOT from wood1
        pos: new Point(parseInt(inputs[3]), parseInt(inputs[4])),
        attackRange: parseInt(inputs[5]),
        health: parseInt(inputs[6]),
        maxHealth: parseInt(inputs[7]),
        shield: parseInt(inputs[8]), // useful in bronze
        attackDamage: parseInt(inputs[9]),
        movementSpeed: parseInt(inputs[10]),
        stunDuration: parseInt(inputs[11]), // useful in bronze
        goldValue: parseInt(inputs[12]),
      };

      const heroData = {
        countDown1: parseInt(inputs[13]), // all countDown and mana const iables are useful starting in bronze
        countDown2: parseInt(inputs[14]),
        countDown3: parseInt(inputs[15]),
        mana: parseInt(inputs[16]),
        maxMana: parseInt(inputs[17]),
        manaRegeneration: parseInt(inputs[18]),
        heroType: inputs[19], // DEADPOOL, VALKYRIE, DOCTOR_STRANGE, HULK, IRONMAN
        isVisible: parseInt(inputs[20]), // 0 if it isn't
        itemsOwned: parseInt(inputs[21]), // useful from wood1
      };

      if (data.type == TYPE_MINION) {
        const minion = new Minion();
        minion.setState({...data});
        this.state.team[data.team].addMinion(minion);
      } else if (data.type == TYPE_HERO) {
        const hero = new Hero();
        hero.setState({...data, ...heroData});
        this.state.team[data.team].addHero(hero);
      } else if (data.type == TYPE_TOWER) {
        const tower = new Tower();
        tower.setState({...data});
        this.state.team[data.team].setState({tower});
      } else if (data.type == TYPE_GROOT) {
        const groot = new Groot();
        groot.setState({...data});
        this.addGroot(groot);
      } else {
        printErr('Unknow entity type ' + data.type);
      }
    }
  }

  updateBushAndSpawn() {
    // usefrul from wood1, represents the number of bushes and the number of places where neutral units can spawn
    const bushAndSpawnPointCount = parseInt(readline());
    for (let i = 0; i < bushAndSpawnPointCount; i++) {
      const inputs = readline().split(' ');
      const type = inputs[0]; // BUSH, from wood1 it can also be SPAWN

      const data = {
        pos: new Point(parseInt(inputs[1]), parseInt(inputs[2])),
        radius: parseInt(inputs[3]),
      };

      if (type == TYPE_BUSH) {
        const bush = new Bush();
        bush.setState({...data});
        this.addBush(bush);
      } else if (type == TYPE_SPAWN) {
        const spawn = new Spawn();
        spawn.setState({...data});
        this.addBush(spawn);
      } else {
        printErr('Unknow bush or spawn type ' + type);
      }
    }
  }

  updateItems() {
    // useful from wood2
    const itemCount = parseInt(readline());
    for (let i = 0; i < itemCount; i++) {
      const inputs = readline().split(' ');

      const data = {
        itemName: inputs[0], // contains keywords such as BRONZE, SILVER and BLADE, BOOTS connected by '_' to help you sort easier
        itemCost: parseInt(inputs[1]), // BRONZE items have lowest cost, the most expensive items are LEGENDARY
        damage: parseInt(inputs[2]), // keyword BLADE is present if the most important item stat is damage
        health: parseInt(inputs[3]),
        maxHealth: parseInt(inputs[4]),
        mana: parseInt(inputs[5]),
        maxMana: parseInt(inputs[6]),
        moveSpeed: parseInt(inputs[7]), // keyword BOOTS is present if the most important item stat is moveSpeed
        manaRegeneration: parseInt(inputs[8]),
        isPotion: parseInt(inputs[9]), // 0 if it's not instantly consumed
      };

      const item = new Item();
      item.setState({...data});
      this.addItem(item);
    }
  }

  sortItems() {
    this.state.items.sort((a, b) => {
      return a.state.itemCost > b.state.itemCost;
    });
  }

  clearTeamUnits() {
    this.state.team.forEach(team => team.clearUnits());
  }

  showItems() {
    this.state.items.forEach(item => {

      let info = '$' + item.state.itemCost + '\t';

      if (item.state.damage > 0) info += '⚔' + item.state.damage + ' '
      else info += '     ';

      if (item.state.health > 0) info += '♡' + item.state.health + ' '
      else info += '     ';

      if (item.state.maxHealth > 0) info += '↑♡' + item.state.maxHealth + ' '
      else info += '      ';

      if (item.state.mana > 0) info += '☯' + item.state.mana + ' '
      else info += '     ';

      if (item.state.maxMana > 0) info += '↑☯' + item.state.maxMana + ' '
      else info += '      ';

      if (item.state.moveSpeed > 0) info += '⤴' + item.state.moveSpeed + ' '
      else info += '     ';

      if (item.state.manaRegeneration > 0) info += '⇈☯' + item.state.manaRegeneration + ' '
      else info += '      ';

      if (item.state.isPotion > 0) info += '☢' + item.state.isPotion + ' '
      else info += '     ';

      info += '\t' + item.state.itemName;

      printErr(info);
    });
  }

  setState(newState) {
    Object.assign(this.state, newState);
  }
}

const game = new Game();
game.updateMyTeam();
game.addTeam();
game.addTeam();
game.updateBushAndSpawn();
game.updateItems();

game.sortItems();
game.showItems();

// game loop
while (true) {
  game.clearTeamUnits();
  game.updateGold();
  game.updateRoundType();
  game.updateEntity();

  if (game.state.roundType <= -2) {
    print(DEADPOOL);
    continue;
  }
  
  if (game.state.roundType <= -1) {
    print(DOCTOR_STRANGE);
    continue;
  }

  const myTeam = game.state.team[game.state.myTeam];
  const myHeroes = myTeam.state.heroes;
  const myMinions = myTeam.state.minions;
  const myTower = myTeam.state.tower;
  const enemyTeam = game.state.team[game.state.enemyTeam];
  const enemyHeroes = enemyTeam.state.heroes;
  const enemyMinions = enemyTeam.state.minions;

  myTeam.updateStatus(enemyTeam);

  myHeroes.forEach((hero) => {
    if (hero.state.status == STATUS_AGGRESSIVE) {
      const target = enemyTeam.heroLowestHealth(enemyHeroes);

      hero.attack(target, 'attack hero ' + target.state.id);
    } else if (hero.state.status == STATUS_RETREAT) {
      hero.moveToBase(myTower, 'retreat to base');
    } else if (hero.state.status == STATUS_FARMING) {
      const minion = hero.farm(enemyMinions);
  
      if (minion) {
        hero.attack(minion, 'atk ' + minion.state.id);
        return;
      }
  
      const minionDeny = hero.deny(myMinions);
  
      if (minionDeny) {
        hero.attack(minionDeny, 'deny ' + minionDeny.state.id);
        return;
      }

      const farmPosition = hero.findFarmPosition(myTeam, enemyTeam);

      if (farmPosition != null) {
        hero.move(farmPosition, 'move to farm position');
        return;
      }
      
      hero.moveToBase(myTower, 'base');
    } else {
      printErr('Unknown hero ' + hero.state.id + ' status');
    }
  });
}