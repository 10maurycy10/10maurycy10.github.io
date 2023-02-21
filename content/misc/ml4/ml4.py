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

lossfn = torch.nn.NLLLoss()
optimizer = torch.optim.Adadelta(model.parameters(), lr=0.1)

mnist = datasets.mnist.MNIST("mnist", train=True, download=True, transform=transform)
mnist_eval = datasets.mnist.MNIST("mnist", train=False, transform=transform)
dataloader = torch.utils.data.DataLoader(mnist, batch_size=64)
dataloader_eval = torch.utils.data.DataLoader(mnist_eval, batch_size=64)


def train():
    model.train()
    for (imgs, target) in tqdm.tqdm(dataloader):
        pred = model(imgs)
        loss = lossfn(pred, target)

        optimizer.zero_grad()
        loss.backward()
        optimizer.step()

def test():
    model.eval()
    correct = 0
    for (imgs, target) in tqdm.tqdm(dataloader_eval):
        pred = model(imgs)
        pred = pred.argmax(dim=1, keepdim=True)
        correct += pred.eq(target.view_as(pred)).sum().item()
    print(f"Acuracy {correct/len(dataloader_eval.dataset) * 100}%")

rounds = 5

test()
for i in range(rounds):
    print(f"Epoch {i}")
    train()
    test()

torch.save(model.state_dict(), "mnist_cnn.pt")



