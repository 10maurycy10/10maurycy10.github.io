---
title: "Detecting text produced by an LLM"
date: 2023-02-28T10:45:18-08:00
tags: ["ai"]
draft: false
---

LLM's (large language models) can generate very convincing text, but this text can easily be detected using a not quite so large language model.
The easiest way to do this is by looking at the [perplexity](https://en.wikipedia.org/wiki/Perplexity) of the text.
This is a number that expresses how good a model (like GPT2) is at predicting a given sample of text.
Perplexity is mostly used for measuring how good LLM's are, but LLM generated texts have a consistently lower perplexity (easier to predict) than text written by a human.
This is because generation is simply a process of repeatedly choosing the word predicted to be most likely by the LLM.

This works even if a different LLM was used for generation then detection, so you can use the smaller and publicly available GPT2 to detect generations from GPT3 (the base of Chat-GPT).
We can use the huggingface transformers library to get a minimal gpt2 model and text tokenizer (A function that converts text into tokens, roughly equivalent to words, and tokens back to text).

```py
import torch
from tqdm import tqdm
from transformers import GPT2LMHeadModel, GPT2TokenizerFast

device = 'cpu'

model = GPT2LMHeadModel.from_pretrained("distilgpt2")
tokenizer = GPT2TokenizerFast.from_pretrained("distilgpt2")
```

Here are two paragraph of text, one written by a human, and one by Open AI's Chat-GPT demo:

```py
human = """
NFC smart cards are commonly used for a wide variety of applications including contactless payments, building access control, and 2 factor authentication. These smart cards have no power source, yet are capable
of bidirectional communication, storing data, and performing cryptographic operations. In each of these cards is a circuit straight out of the 1900s: The crystal radio.
Most of the card is taken up by a flat coil, which can often be seen if the card is held up to a bright light. Another component you can easily see this way is a capacitor, it looks like a large (usually square) opaque region. The capacitor is placed in parallel with the coil, forming a parallel tuned circuit (resonant at 13.56 MHz), similar to a crystal radio.
"""

gpt = """
Near Field Communication (NFC) is a type of wireless communication that allows two electronic devices to communicate with each other over short distances. It uses electromagnetic induction to establish a wireless link between two devices by bringing them close together. In NFC, a transmitter sends out a weak radio frequency signal that can be picked up by a receiver. The receiver then converts this signal into data, which can be used to perform various functions such as data exchange, contactless payments, and access control.
"""
```

To compute perplexity, first extract sequences of tokens (chunks of text somewhat equivalent to words) along with the token that follows the sequence.

```py
def perplexity(encodings, model, stride=5, max_length=None):
    """
    The stride paramteter controls how freqently pedictions are made and compaired with the text.
    Higher values are faster but less acurate.
    """
    max_length = max_length or model.config.n_positions
    seq_len = encodings.input_ids.size(1)

    nlls = []
    prev_end_loc = 0
    for begin_loc in tqdm(range(0, seq_len, stride)):
        end_loc = min(begin_loc + max_length, seq_len)
        input_ids = encodings.input_ids[:, begin_loc:end_loc].to(device)
```

Now generate the target, padding with -100 placeholder tokens.

```py
        trg_len = end_loc - prev_end_loc  # may be different from stride on last loop
        target_ids = input_ids.clone()
        target_ids[:, :-trg_len] = -100
```

Run the model to get the loss function.

```py
        with torch.no_grad():
            outputs = model(input_ids, labels=target_ids)

            # loss for GPT2 is CrossEntropyLoss which averages over input tokens.
            # Multiply it with trg_len to get the summation instead of average.
            neg_log_likelihood = outputs.loss * trg_len

        nlls.append(neg_log_likelihood)

        prev_end_loc = end_loc
        if end_loc == seq_len:
            break

```

Finally, compute the over all perplexity:

```py
    return torch.exp(torch.stack(nlls).sum() / end_loc)
```

Testing it:

```py
def get_score(text):
    encodings = tokenizer(text, return_tensors="pt")
    return perplexity(encodings,model, max_length=20)

print("GPT\t",get_score(gpt))
print("Human\t",get_score(human))
```

The human text gets a perplexity rating of 59.4741, while the GPT generated text gets 18.4398.

