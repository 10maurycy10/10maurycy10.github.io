import diffusers
from transformers import CLIPTextModel, CLIPTokenizer
from diffusers import AutoencoderKL, UNet2DConditionModel, LMSDiscreteScheduler
import torch
import tqdm
import numpy as np
from PIL import Image

# The autoencoder for working with latent space
vae = AutoencoderKL.from_pretrained("CompVis/stable-diffusion-v1-4", subfolder="vae")

# The text encoder and tokenizer from CLIP
tokenizer = CLIPTokenizer.from_pretrained("openai/clip-vit-large-patch14")
text_encoder = CLIPTextModel.from_pretrained("openai/clip-vit-large-patch14")

# The unet model for diffusions.
# This consists of an encoder that downsamples the latent space, and then reverses the process.
# The model contains short cut connections to avoid loss of detail, and uses cross attention to include the text embeddigns
unet = UNet2DConditionModel.from_pretrained("CompVis/stable-diffusion-v1-4", subfolder="unet")

torch_device = "cuda" if torch.cuda.is_available() else "cpu"
vae.to(torch_device)
text_encoder.to(torch_device)
unet.to(torch_device)

scheduler = LMSDiscreteScheduler(beta_start=0.00085, beta_end=0.012, beta_schedule="scaled_linear", num_train_timesteps=1000)

# Most of the models run on batches, passing multiple prompts will provide multiple images.
prompt = ["a photograph of an astronaut riding a horse"]

# 512*512 is a resonable balance of resolution and time to generate
height = 512                        
width = 512                         

# How many steps to denoise for
num_inference_steps = 20       

# Guidence scale, controls the degree of guidence for the diffuser, higher values more closely follow the prompt, but are less creative
guidance_scale = 8

# Create an RNG for generating the noise image, using a different value to get a different image
# Using the same value allows for reproducable results, allowing adjustment of the prompt.
rng = torch.manual_seed(42)

# How many images to generate
batch_size = len(prompt)

text_input = tokenizer(prompt, padding="max_length", max_length=tokenizer.model_max_length, truncation=True, return_tensors="pt")

with torch.no_grad():
  text_embeddings = text_encoder(text_input.input_ids.to(torch_device))[0]

max_length = text_input.input_ids.shape[-1]

uncond_input = tokenizer(
    [""] * batch_size, padding="max_length", max_length=max_length, return_tensors="pt"
)

with torch.no_grad():
  uncond_embeddings = text_encoder(uncond_input.input_ids.to(torch_device))[0] 

text_embeddings = torch.cat([uncond_embeddings, text_embeddings])

latents = torch.randn(
  (batch_size, unet.in_channels, height // 8, width // 8),
  generator=rng,
)
latents = latents.to(torch_device)

# Tell the schedualer to compute the sigma values
scheduler.set_timesteps(num_inference_steps)

# Scale the latents before diffusion, K-LMS specific
latents = latents * scheduler.init_noise_sigma

# For each denoising step...
for (i, sigma) in tqdm.tqdm(enumerate(scheduler.timesteps)):
    # Expand the latents so that each latent image gets run both with the prompt and the uncond_embeddings
    latent_model_input = torch.cat([latents] * 2)

    # The latents are scalled, make sure to undo this before feeding into unet.
    latent_model_input = scheduler.scale_model_input(latent_model_input, sigma)

    # Use unet to generate the noise prediction
    with torch.no_grad():
        noise_pred = unet(latent_model_input, sigma, encoder_hidden_states=text_embeddings).sample

    # Seperate the conditioned and unconditioned predictions
    noise_pred_uncond, noise_pred_text = noise_pred.chunk(2)

    # Scale the diffences between conditioned and unconditioned prediction by the guidence scale
    noise_pred = noise_pred_uncond + guidance_scale * (noise_pred_text - noise_pred_uncond)

    # Finaly, use the schedualer to merge the prediction into the latent space
    latents = scheduler.step(noise_pred, sigma, latents).prev_sample

latents = 1/0.18215 * latents

# Run vae to decode image
with torch.no_grad():
  image = vae.decode(latents).sample

image = (image / 2 + 0.5).clamp(0, 1)
image = image.detach().cpu().permute(0, 2, 3, 1).numpy()
images = (image * 255).round().astype("uint8")

pil_images = [Image.fromarray(image) for image in images]
pil_images[0].save("out.png")
