const STRAIGTH_ANGLE = 180;
const QUADRILATERAL = 'quadrilateral';
const PARALLELOGRAM = 'parallelogram';
const RHOMBUS = 'rhombus';
const RECTANGLE = 'rectangle';
const SQUARE = 'square';

class Vertex {
    constructor(point, x, y) {
        this.point = point;
        this.x = x;
        this.y = y;
    }
    
    distance(vertex) {
        const x = this.x - vertex.x;
        const y = this.y - vertex.y;
        
        return Math.sqrt(x * x + y * y);
    }
    
    angle(vertexA, vertexB) {
        const a = this.distance(vertexA);
        const b = this.distance(vertexB);
        const c = vertexA.distance(vertexB);

        const cos = Math.sqrt(c*c - (a*a + b*b) / ((-2) * a * b));

        return Math.acos(cos * STRAIGTH_ANGLE / Math.PI);
    }
}

class Polygon {
    constructor(vertices) {
        this.vertices = vertices;
    }
    
    getPoints() {
        const points =  this.vertices.map((vertice) => {
            return vertice.point;
        });

        return points.join('');
    }

    getSides() {
        return this.vertices.reduce(function(distances, vertex, index, vertices) {
            if(index + 1 >= vertices.length) {
                return distances;
            }

            const chunk = vertices.slice(index + 1);
            return chunk.reduce(function(distances, vertex) {
                distances.push(this.distance(vertex));
                return distances;
            }.bind(vertex), distances);
        }, []);
    }

    countDistance(counts, distance) {
        counts[distance] = (counts[distance] || 0) + 1;
        return counts;
    }
    
    getAngles() {
        const angles = [];
        
        for(let i = 0; i < this.vertices.length; i++) {
            for(let j = 0; j < this.vertices.length; j++) {
                const angle = this.vertices[i].angle(this.vertices[j]);
                angles.push(angle);
            }
        }

        return angles;
    }

    isSquare(sidesCount) {
        return sidesCount.some(function(side) {
            return side >= 4;
        });
    }

    quadrilateralName() {
        const sides = this.getSides();
        const sidesCount = Object.values(sides.reduce(this.countDistance, {}));
        //const angles = this.getAngles();

        printErr(sidesCount);
        //printErr(angles);

        if(this.isSquare(sidesCount)) {
            return SQUARE;
        }
    }
}

const n = parseInt(readline());

for(let i = 0; i < n; i++) {
    const inputs = readline().split(' ');
    const vertices = [];
    
    vertices.push(new Vertex(inputs[0], inputs[1], inputs[2]));
    vertices.push(new Vertex(inputs[3], inputs[4], inputs[5]));
    vertices.push(new Vertex(inputs[6], inputs[7], inputs[8]));
    vertices.push(new Vertex(inputs[9], inputs[10], inputs[11]));
    
    const polygon = new Polygon(vertices);

    const name = polygon.quadrilateralName();
    print(polygon.getPoints() + ' is a ' + name + '.');
}