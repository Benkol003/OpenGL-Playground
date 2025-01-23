import pygame

import math

class Vec3(object):
    def __init__(self, x, y, z):
        self.x = x
        self.y = y
        self.z = z

    @staticmethod
    def dot(v1, v2):
        return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z

    @staticmethod
    def cross(v1, v2):
        x = v1.y * v2.z - v1.z * v2.y
        y = v1.x * v2.x - v1.x * v2.z
        z = v1.z * v2.y - v1.y * v2.x
        return Vec3(x, y, z)

    @staticmethod
    def normalize(v):
        return v / v.norm()

    def norm(self):
        return math.sqrt(Vec3.dot(self, self))

    def __add__(self, v):
        return Vec3(self.x + v.x, self.y + v.y, self.z + v.z)

    def __neg__(self, v):
        return Vec3(-self.x, -self.y, -self.z)

    def __sub__(self, v):
        return self + (-v)

    def __mul__(self, v):
        if isinstance(v, Vec3):
            return Vec3(self.x * v.x, self.y * v.y, self.z * v.z)
        else:
            return Vec3(self.x * v, self.y * v, self.z * v)

    def __rmul__(self, v):
        return self.__mul__(v)

    def __div__(self, v):
        if isinstance(v, Vec3):
            return Vec3(self.x / v.x, self.y / v.y, self.z / v.z)
        else:
            return Vec3(self.x / v, self.y / v, self.z / v)

    def __str__(self):
        return '[ %.4f, %.4f, %.4f ]' % (self.x, self.y, self.z)


class Vec2(object):
    def __init__(self,x,y):
        self.x=x
        self.y=y

    @staticmethod
    def dot(v1, v2):
        return v1.x * v2.x + v1.y * v2.y

    @staticmethod
    def normalize(v):
        return v / v.norm()

    def norm(self):
        return math.sqrt(Vec2.dot(self, self))

    def __add__(self, v):
        return Vec3(self.x + v.x, self.y + v.y)

    def __neg__(self, v):
        return Vec3(-self.x, -self.y)

    def __sub__(self, v):
        return self + (-v)

    def __mul__(self, v):
        if isinstance(v, Vec2):
            return Vec2(self.x * v.x, self.y * v.y)
        else:
            return Vec2(self.x * v, self.y * v)

    def __rmul__(self, v):
        return self.__mul__(v)

    def __div__(self, v):
        if isinstance(v, Vec2):
            return Vec3(self.x / v.x, self.y / v.y)
        else:
            return Vec3(self.x / v, self.y / v)

    def __str__(self):
        return '[ %.4f, %.4f]' % (self.x, self.y)

def side(a,b,x):
    ax = Vec2(x.x-a.x,x.y-a.y)
    xb = Vec2(b.x-x.x,b.y-x.y)

    return (ax.x*xb.y)-(xb.x*ax.y) > 0
    #return Vec3.dot(Vec3.cross(b-a,x-a),vec3(1,1,1)) > 0;


def in_triangle(a,b,c,x):
    ab_s = side(a,b,x)
    ac_s = side(a,c,x)
    if(ab_s==ac_s):
        return False #not inside cone of ab / ac

    #side of vec BC (direction matters, not CB)= side of AB
    return side(b,c,x)==ab_s





(width, height) = (600, 600)
screen = pygame.display.set_mode((width, height))
pygame.display.flip()

a = Vec3(100,100,0)
b = Vec3(500,100,0)
c = Vec3(300,500,0)

pygame.draw.lines(screen, (0, 0, 255),True, ((a.x,a.y),(b.x,b.y),(c.x,c.y)))

for xi in range(600):
    for yi in range(600):
        if in_triangle(a,b,c,Vec3(xi,yi,0)):
            screen.set_at((xi, yi),(0,255,0))
        else:
            screen.set_at((xi, yi),(255,0,0))

pygame.display.flip()

while(True):
    pygame.display.flip()