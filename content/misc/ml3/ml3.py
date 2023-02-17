import torch
import numpy as np
from torch import nn
import tqdm

model = nn.Sequential(
    nn.Linear(1, 16),
    nn.ReLU(),
    nn.Linear(16, 16),
    nn.ReLU(),
    nn.Linear(16, 1),
)

print(model)
#print(model(torch.tensor([1]).to(torch.float)))

data = np.random.rand(2, 200)
data[0] += 1
data[1] = np.sin(data[0] * 5)

x = torch.from_numpy(data[0]).reshape(200,1).to(torch.float)
y = torch.from_numpy(data[1]).reshape(200,1).to(torch.float)

loss_fn = nn.MSELoss()
optimizer = torch.optim.SGD(model.parameters(), lr=1e-1)

steps = 500
for i in tqdm.tqdm(range(steps)):
    pred = model(x)
    loss = loss_fn(pred, y)

    optimizer.zero_grad()
    loss.backward()
    optimizer.step()

import matplotlib.pyplot

prediction = model(x)
matplotlib.pyplot.scatter(data[0],data[1])
matplotlib.pyplot.scatter(data[0],prediction.detach().numpy())
matplotlib.pyplot.savefig("final.png")

print(loss)
