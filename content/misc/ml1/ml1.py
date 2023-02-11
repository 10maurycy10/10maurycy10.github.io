from numpy import random
import numpy as np
import matplotlib.pyplot

colors = ["red", "blue"]

random_points = random.rand(2, 100)
data = random_points

data[1] = data[0] * 4 + data[1]

matplotlib.pyplot.scatter(data[0],data[1])
matplotlib.pyplot.savefig("data.png")

# The parameters are bundled together in a numpy array
init_params = np.array([1, 0])

def model(x, params):
    return params[0] * x + params[1]

prediction = []
for x in data[0]:
    prediction.append(model(x, init_params))

matplotlib.pyplot.scatter(data[0],prediction)
matplotlib.pyplot.savefig("init.png")

def error(x, y, params):
    return model(x, params) - y

def diriv(x, params):
    """
    params is unused for now
    With y = ax + b, a change in b will always cause an identical change in y, making the diriverative 1
    For a, a change's effect is determined by the value of x.
    """
    return np.array([x,1])

def grad(x, y, params):
    return diriv(x, params) * error(x, y, params)

def loss_grad(dataset, params):
    """
    Computes the parameter gradents for the entire dataset
    """
    grads = [grad(dataset[0][i], dataset[1][i], params) for i in range(len(dataset[0]))]
    return np.mean(grads, axis=0)

params = init_params

learn_rate = 0.5
steps = 50
for i in range(steps):
    params = params + learn_rate * -loss_grad(data, params)

prediction = []
for x in data[0]:
    prediction.append(model(x, init_params))

matplotlib.pyplot.scatter(data[0],prediction)
matplotlib.pyplot.savefig("final.png")
