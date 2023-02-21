---
title: "Image classification, on the MNIST dataset [ML4]"
date: 2023-02-17T13:37:34-08:00
tags: ["ai"]
draft: false
---

A common example of a problem that can be solved using ML, but is difficult otherwise is handwriting recognition.
But before we can try, we first need to find a dataset, I will be using the MNIST dataset, containing 28x28 images of handwritten digits.
The torchvision package provides an easy way to get it, but it must first be installed with pip:

```sh
pip install torchvision
```

Now MNIST can trivially be downloaded and loaded, but we need to convert it to an torch tensor from a PIL image.
MNIST is split into separate datasets for training and evaluation, so that the network does not memorize the training data and give deceptive results when tested, so lets load both:

```py
import torch
from torch import nn
from torchvision import datasets, tranforms
import tqdm

mnist = datasets.mnist.MNIST("mnist", train=True, download=True, transform=transforms.ToTensor())
mnist_eval = datasets.mnist.MNIST("mnist", train=False, transform=transforms.ToTensor())
```

A dataloader allows the data to be batched to avoid iterating over each example in python (which would be slow):

```py
batch_size=64

dataloader = torch.utils.data.DataLoader(mnist, batch_size=batch_size)
dataloader_eval = torch.utils.data.DataLoader(mnist_eval, batch_size=batch_size)
```

The NLL loss function is designed for training classifier networks, where there is an output for every category, and the highest output is the predicted category.

```py
lossfn = torch.nn.NLLLoss()
optimizer = torch.optim.SGD(model.parameters(), lr=0.1)
```

Even with MNIST's low resolution images, a densely connected layer would be huge.
Instead, a Convolutional Neural Network (CNN) can be used, where a tiny network that converts a small (3x3 and 5x5 in my model) area of pixels into a handful of outputs (32 and 16).
This network is then run for every location on the input image to generate an output image, vastly reducing the amount of parameters required.
After a few convectional Neural Network's, a few densely connected layers reduce the processed image into a predicted digit.
Softmax converts the raw outputs into a set of values ranging from 0 to 1.

```
model = nn.Sequential(
    torch.nn.Conv2d(1,32, kernel_size=3),
    torch.nn.ReLU(),
    torch.nn.Conv2d(32,16, kernel_size=5),
    torch.nn.ReLU(),
    torch.nn.Flatten(),
    torch.nn.Linear(7744,128),
    torch.nn.ReLU(),
    torch.nn.Linear(128,10),
    torch.nn.Softmax()
)
```

Now we can implement the training loop:

```py
def train():
    # Some layers like dropout behave differently during eval and training, we need to let them know
    model.train()
    for (imgs, target) in tqdm.tqdm(dataloader):
        pred = model(imgs)
        loss = lossfn(pred, target)

        optimizer.zero_grad()
        loss.backward()
        optimizer.step()
```

Using the evaluation dataset, we can see how the network does on inputs it has never seen before:

```py
def test():
    model.eval()
    correct = 0
    for (imgs, target) in tqdm.tqdm(dataloader_eval):
        pred = model(imgs)
        pred = pred.argmax(dim=1, keepdim=True)
        correct += pred.eq(target.view_as(pred)).sum().item()
    print(f"Acuracy {correct/len(dataloader_eval.dataset) * 100}%")
```

Time to actually train the model:

```py
rounds = 5

test()
for i in range(rounds):
    print(f"Epoch {i}")
    train()
    test()
```

This can achieve an accuracy of 96% with just a few minuets of training.
Torch provides an easy way to save the model for later use:

```
torch.save(model.state_dict(), "mnist_cnn.pt")
```

# Using the model

Once you have trained the model, you can use it.
First we need the untrained model, this must be identical to the one that was trained:

```py
import torch
from torch import nn
from torchvision import transforms
import PIL

transform = transforms.ToTensor(),

model = nn.Sequential(
    torch.nn.Conv2d(1,32, kernel_size=3),
    torch.nn.ReLU(),
    torch.nn.Conv2d(32,16, kernel_size=5),
    torch.nn.ReLU(),
    torch.nn.Flatten(),
    torch.nn.Linear(7744,128),
    torch.nn.ReLU(),
    torch.nn.Linear(128,10),
    torch.nn.Softmax()
)
```

Now we can load the file created after training:

```py
model.load_state_dict(torch.load("mnist_cnn.pt"))
model.eval()
```

We need to ensure the image is 28x28 and gray scaled, then we can run the model:

```py
def classify(img):
    img = img.resize((28,28))
    img = img.convert("L")
    model_input = transform(img).reshape(1,1,28,28)
    prediction = model(model_input).argmax(dim=1)
    return prediction
```

Running this model is quite easy with our utility function:

```py
img = PIL.Image.open("six.png")
print(classify(img))
```

![Crude white on back drawing of a figure six](six.png)
This image get classified as a `6`, success!

Here is the full code for [traning](ml4.py) and [running](ml4_realworld.py).
