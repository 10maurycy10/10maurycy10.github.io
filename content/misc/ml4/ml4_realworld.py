import torch
from torch import nn
from torchvision import transforms, datasets
import tqdm

transform = transforms.Compose([
    transforms.ToTensor(),
])

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

model.load_state_dict(torch.load("mnist_cnn.pt"))

model.eval()

import PIL

def classify(img):
    img = img.resize((28,28))
    img = img.convert("L") 
    model_input = transform(img).reshape(1,1,28,28)
    with torch.no_grad():
        prediction = model(model_input).argmax(dim=1)
    return prediction

images = [
    "digits/zero.png",
    "digits/one.png",
    "digits/two.png",
    "digits/three.png",
    "digits/four.png",
    "digits/five.png",
    "digits/six.png",
    "digits/seven.png",
    "digits/eight.png",
    "digits/nine.png"
]

for name in images:
    img = PIL.Image.open(name)
    print(name,classify(img))
