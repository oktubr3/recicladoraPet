
import FreeCAD as App
import Part

doc = App.newDocument("CaseDisplay1602")

# Base inclinada tipo rampa
base = Part.makeBox(100, 60, 30)  # ancho, profundo, alto
base.translate(App.Vector(-50, -30, 0))  # centrar en X y Y

# Crear el plano inclinado al frente (frontal más bajo)
# Esto se simula restando un bloque diagonal
corte_inclinacion = Part.makeBox(100, 60, 30)
corte_inclinacion.rotate(App.Vector(0,0,0), App.Vector(1,0,0), -45)
corte_inclinacion.translate(App.Vector(-50, -30, 0))
base = base.cut(corte_inclinacion)

# Corte para el LCD (72x26 mm)
lcd = Part.makeBox(72, 5, 26)
lcd.rotate(App.Vector(0,0,0), App.Vector(1,0,0), -45)
lcd.translate(App.Vector(-36, -2, 10))
base = base.cut(lcd)

# 4 botones (agujeros de Ø16 mm)
for i in range(4):
    x = (i - 1.5) * 20
    cyl = Part.makeCylinder(8, 30, App.Vector(x, -25, 5), App.Vector(0,0,1))
    base = base.cut(cyl)

# Potenciómetro - eje central
pot = Part.makeCylinder(4.1, 30, App.Vector(-45, -25, 5), App.Vector(0,0,1))
base = base.cut(pot)

# Agujeros de fijación del potenciómetro
for dx in [-5.7, 5.7]:
    fix = Part.makeCylinder(1.3, 30, App.Vector(-45, -25 + dx, 5), App.Vector(0,0,1))
    base = base.cut(fix)

# Crear objeto final en el documento
part_obj = doc.addObject("Part::Feature", "CaseDisplay")
part_obj.Shape = base

App.ActiveDocument.recompute()
