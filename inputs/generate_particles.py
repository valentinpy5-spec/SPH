rho0, dx = 1000.0, 0.03

fluid = []
boundary = []

m = rho0 * dx**3  

nb = int(1.0 / dx) + 4

nx, ny, nz = 20, 15, 15
for i in range(nx):
    for j in range(ny):
        for k in range(nz):
            fluid.append((dx + i*dx, dx + j*dx, dx + k*dx, m, "false"))


def add(x, y, z):
    boundary.append((x, y, z, m, "true"))

# for a in range(-2, nb):
#     pa = a * dx
#     for b in range(-2, nb):
#         pb = b * dx
#         for layer in (0, 1):          
#             d = layer * dx
#             add(pa, -d,      pb)       
#             add(pa, 1.0 + d, pb)       
#             add(-d,      pa, pb)      
#             add(1.0 + d, pa, pb)       
#             add(pa, pb, -d)            
#             add(pa, pb, 1.0 + d)   

all_particles = fluid + boundary

with open("./dam_break.in", "w") as f:
    f.write(f"{len(all_particles)}\n")
    for (x, y, z, mass, isb) in all_particles:
        f.write(f"{x:.4f} {y:.4f} {z:.4f} 0.0 0.0 1.0 {mass:.6f} {isb}\n")

print(f"Fluide   : {len(fluid)}")
print(f"Boundary : {len(boundary)}")
print(f"Total    : {len(all_particles)}")
