import imageio.v2 as imageio

images = [f"images/path_test_{20*x}.png" for x in range(25)]
frames = []

for f in images:
    img = imageio.imread(f)
    frames.append(img)
imageio.mimsave('path.gif', frames, fps=15)