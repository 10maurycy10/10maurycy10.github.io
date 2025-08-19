---
title: "But how do lenses do that?"
date: 2025-08-17
---

[In a recent article](/projects/take_a_photo/), I showed how a lens can project an image onto light sensitive chemiclas to take a photograph, but glossed over exactly *how* a curved piece of glass can do that. 

# What even is light?

When a charged particle, like an electron, accelerates, some of that force is transfered to nearby particles:

<canvas id=anim1 style="width: 40em; height: 20em;">
(This animation won't work over RSS. Please use the <a href=/misc/lenses/>HTML version</a>.)
</canvas>

<script defer>
var start = new Date();

function smooth(x0, x1, a) {
	var a = (Math.atan(a) + 1)/2;
	return a*x1 + (1 - a)*x0
}

function lerp(x0, x1, a) {
	return a*x1 + (1 - a)*x0
}

const electron = new Image();
electron.src = "electron.png"

function anim1() {
	var canvas = document.getElementById("anim1");
	var ctx = canvas.getContext("2d");
	function draw() {
		var w = canvas.getBoundingClientRect().width;
		var h = canvas.getBoundingClientRect().height;
		canvas.width = w;
		canvas.height = h;
		//ctx.globalCompositeOperation = "destination-over";
		// Seconds
		var time = ((new Date()) - start) / 1000; 
		// Repeat every 5 seconds
		time %= 5;
		time /= 5;
	
		function center_img() {
			ctx.translate(-64, -64);
		}
		
		ctx.fillStyle = "rgb(10 10 10)";
		ctx.fillRect(0, 0, w, h);
		
		// Wave progress
		var progress = (time - 0.28) / (0.7 - 0.3);
		if (progress < 0) progress = 0;
		var bar = lerp(0, 400, progress)
		// First particle
		ctx.save()
		// Wave
		ctx.fillStyle = "rgb(20 20 20)";
		ctx.strokeStyle = "rgb(50 50 50)";
		ctx.lineWidth = 30;
		ctx.beginPath();
		ctx.arc(w/2 - 200, h/2, bar, Math.PI * 2, 0)
		ctx.stroke()
		// Particle cont.
		ctx.translate(0, smooth(25, -25, (time - .3)*50))
		center_img()
		ctx.drawImage(electron, w/2 - 200, h/2);
		ctx.restore()
		// Second particle
		ctx.save()
		ctx.translate(0, smooth(10, -10, (time - .7)*50))
		center_img()
		ctx.drawImage(electron, w/2 + 200, h/2);
		ctx.restore()
		window.requestAnimationFrame(draw);
	}
	window.requestAnimationFrame(draw)
}
anim1();
</script>

The interaction isn't instant, it has a delay of around 3 nanoseconds for every meter of distance.
This electromagnetic wave falls off much slower then the electrostiatic field, and can be felt much futher away.

We call these waves light.

# Metal:

In metals, where electrons are able to move freely, the induced movements create their own wave:

<canvas id=anim2 style="width: 40em; height: 20em;">
</canvas>
<center style="color: gray">EM wave interacting with a conductor</center>

<script defer>
function anim2() {
	var canvas = document.getElementById("anim2");
	var ctx = canvas.getContext("2d");
	function draw() {
		var w = canvas.getBoundingClientRect().width;
		var h = canvas.getBoundingClientRect().height;
		canvas.width = w;
		canvas.height = h;
		//ctx.globalCompositeOperation = "destination-over";
		// Seconds
		var time = ((new Date()) - start) / 1000; 
		// Repeat every 5 seconds
		time %= 5;
		time /= 5;
	
		function center_img() {
			ctx.translate(-64, -64);
		}
		
		ctx.fillStyle = "rgb(10 10 10)";
		ctx.fillRect(0, 0, w, h);
		
		// Wave progress
		var progress = (time - 0.28) / (0.7 - 0.3);
		if (progress < 0) progress = 0;
		var bar = lerp(0, 400, progress)
		// Wave progress
		var progress2 = (time - 0.48) / (0.7 - 0.3);
		if (progress2 < 0) progress2 = 0;
		var bar2 = lerp(0, 400, progress2)
		// First particle
		ctx.save()
		// Wave
		ctx.fillStyle = "rgb(20 20 20)";
		ctx.strokeStyle = "rgb(50 50 50)";
		ctx.lineWidth = 30;
		ctx.beginPath();
		ctx.arc(w/2 - 200, h/2, bar, Math.PI * 2, 0)
		ctx.stroke()
		// Particle cont.
		ctx.translate(0, smooth(25, -25, (time - .3)*50))
		center_img()
		ctx.drawImage(electron, w/2 - 200, h/2);
		ctx.restore()
		// Second particle
		ctx.save()
		// Wave 2
		ctx.globalCompositeOperation = "difference"
		ctx.fillStyle = "rgb(20 20 20)";
		ctx.strokeStyle = "rgb(50 50 50)";
		ctx.lineWidth = 30;
		ctx.beginPath();
		ctx.arc(w/2, h/2, bar2, Math.PI * 2, 0)
		ctx.stroke()
		// Particle cont
		ctx.translate(0, smooth(10, -10, (time - .5)*50))
		center_img()
		ctx.drawImage(electron, w/2, h/2);
		ctx.restore()
//		// Rest of the particles
//		for (var i = 1; i < 5; i++) {
//			ctx.save()
//			center_img()
//			ctx.drawImage(electron, w/2 + i * 50, h/2);
//			ctx.restore()
//		}
		window.requestAnimationFrame(draw);
	}
	window.requestAnimationFrame(draw)
}
anim2()
</script>

Here's the same thing drawn as a graph of electric field (force that would be experienced by an imaginary particle) across a one dimentional slice of space:

<canvas id=anim1.5 style="width: 40em; height: 20em;">
</canvas>

<script defer>
function anim_one_half() {
	var canvas = document.getElementById("anim1.5");
	var ctx = canvas.getContext("2d");
	function draw() {
		var w = canvas.getBoundingClientRect().width;
		var h = canvas.getBoundingClientRect().height;
		canvas.width = w;
		canvas.height = h;
		ctx.fillStyle = "rgb(10 10 10)";
		ctx.fillRect(0, 0, w, h);
		// Seconds
		var time = ((new Date()) - start) / 1000; 
		time %= 5;
	
		ctx.lineWidth = 4;

		function wave(a) {
			return -1 / (a*a + 1);
		}

		function incedent(x, t) {
			var dx = x - 0.15;
			var phase  = dx * 30 + -t * 5;
			var phase2 = dx * 30 + t * 5;
			return wave(phase) + wave(phase2)
		}

		function reflected(x, t) {
			if (x > 0.5) return -incedent(x, t);
			else return -incedent(-x + 1, t);
		}

		// Reflected wave
		ctx.beginPath()
		for (var i = 0; i < 255; i++) {
			var x = (i / 255);
			var y = reflected(x, time);
			y /= 5;
			y += 0.5;
			ctx.lineTo(x * w, y * h);
			if (i == 128) {
				ctx.strokeStyle = "rgb(10 200 200)";
				ctx.stroke();
				ctx.beginPath()
			}
		}
		ctx.strokeStyle = "rgb(10 100 100)";
		ctx.stroke();
		// Combined wave
		ctx.beginPath()
		for (var i = 0; i < 255; i++) {
			var x = (i / 255);
			var y = reflected(x, time) + incedent(x, time);
			y /= 5;
			y += 0.5;
			ctx.lineTo(x * w, y * h);
		}
		ctx.strokeStyle = "rgb(255 255 255)";
		ctx.stroke();
		// Incedent wave
		ctx.beginPath()
		for (var i = 0; i < 255; i++) {
			var x = (i / 255);
			var y = incedent(x, time);
			y /= 5;
			y += 0.5;
			ctx.lineTo(x * w, y * h);
			if (i == 128) {
				ctx.strokeStyle = "rgb(255 255 10)";
				ctx.stroke();
				ctx.beginPath()
			}
		}
		ctx.strokeStyle = "rgb(100 100 10)";
		ctx.stroke();
		// Section
		ctx.fillStyle = "rgb(250 250 255)";
		ctx.fillRect(w/2 - 5, 0, 10, h)
		// Lables
		ctx.font = "" + h/20 + "px sans";
		ctx.fillStyle = "rgb(255 250 0)";
		ctx.fillText("Yellow", 0, h/20);
		ctx.fillStyle = "rgb(0 250 255)";
		ctx.fillText("Cyan", 0, h/20 * 2 + 4);
		ctx.fillStyle = "rgb(250 250 255)";
		ctx.fillText("White", 0, h/20 * 3 + 8);
		ctx.fillText("Incident wave", h/5, h/20);
		ctx.fillText("Re-radiated", h/5, h/20 * 2 + 4);
		ctx.fillText("Combined", h/5, h/20 * 3 + 8);
		
		window.requestAnimationFrame(draw);
	}
	window.requestAnimationFrame(draw)
}
anim_one_half()
</script>
<br>
<br>
<div style="border: 1px yellow solid; padding: .5em;">
Remember that the wave isn't moving side to side, only the graph that does that.
</div>

While both waves travel infinatly, behind the material, the two forces cancel out and have no affect: a shadow. 
Everywhere else, the waves are out of sync, and both are detectible: a reflection.

# Glass:

Ok, but most materials aren't metals, and the electron's aren't free to move. 
For our purposes, it's enough to imaging bound electrons as tiny charged balls on a spring, who's movement lags behind the electromagnetic field. 

Here's what happens to a wave as it crosses a slice of dialectric material like glass:

<canvas id=anim3 style="width: 40em; height: 20em;">
</canvas>
<center style="color: gray">Reflection not shown</center>

<script defer>
function anim3() {
	var canvas = document.getElementById("anim3");
	var ctx = canvas.getContext("2d");
	function draw() {
		var w = canvas.getBoundingClientRect().width;
		var h = canvas.getBoundingClientRect().height;
		canvas.width = w;
		canvas.height = h;
		ctx.fillStyle = "rgb(10 10 10)";
		ctx.fillRect(0, 0, w, h);
		// Seconds
		var time = ((new Date()) - start) / 1000; 
	
		ctx.lineWidth = 4;

		function incedent(x, t) {
			var phase = x * 20 + -t * 3;
			return Math.sin(phase);
		}

		function reflection(x, t) {
			t += 3.14 / 3 * 0.4;
			if (x > 100/255) {
				return -incedent(x, t) / 5;
			} else {
				return incedent(-x, t) / 5;
			}
		}

		// Incedent wave
		ctx.beginPath()
		for (var i = 0; i < 255; i++) {
			var x = (i / 255);
			var y = incedent(x, time);
			y = y / 4 + 0.5;
			ctx.lineTo(x * w, y * h);
			if (i == 100) {
				ctx.strokeStyle = "rgb(250 250 10)";
				ctx.stroke();
				ctx.beginPath()
			}
		}
		ctx.strokeStyle = "rgb(100 100 10)";
		ctx.stroke();
		// Remited wave
		var shift = Math.PI + Math.PI * 0.45
		ctx.beginPath()
		ctx.strokeStyle = "rgb(0 100 100)";
		for (var i = 0; i < 255; i++) {
			var x = (i / 255);
			var y = reflection(x, time)/4 + .5;
			ctx.lineTo(x * w, y * h);
		}
		ctx.stroke();
		// Combined wave
		ctx.beginPath()
		ctx.strokeStyle = "rgb(255 250 250)";
		for (var i = 0; i < 255; i++) {
			var x = (i / 255);
			var phase = x * 20 + -time * 3 - shift;
			var y = reflection(x, time) + incedent(x, time);
			y = y/4 + 0.5;
			ctx.lineTo(x * w, y * h);
		}
		ctx.stroke();
		// Section
		ctx.fillStyle = "rgb(100 100 100)";
		ctx.fillRect(100/255 * w - 5, 0, w/255 + 5, h)
		// Lables
		ctx.font = "" + h/20 + "px sans";
		ctx.fillStyle = "rgb(255 250 0)";
		ctx.fillText("Yellow", 0, h/20);
		ctx.fillStyle = "rgb(0 250 255)";
		ctx.fillText("Cyan", 0, h/20 * 2 + 4);
		ctx.fillStyle = "rgb(250 250 255)";
		ctx.fillText("White", 0, h/20 * 3 + 8);
		ctx.fillText("Incident wave", h/5, h/20);
		ctx.fillText("Re-radiated", h/5, h/20 * 2 + 4);
		ctx.fillText("Combined", h/5, h/20 * 3 + 8);
		window.requestAnimationFrame(draw);
	}
	window.requestAnimationFrame(draw)
}
anim3()
</script>
<br>

The distracting wobblieness on the left is reflected light interfering with the incomming light. 
What's important is that the wave that passes though is slightly pushed back in phase from the orignal.

When traveling though bulk material,the tiny phase kicks result in the wave apearing to travel slower:

<canvas id=anim4 style="width: 40em; height: 20em;">
</canvas>
<center style="color: gray">Reflection not shown</center>

<script defer>
function anim4() {
	var canvas = document.getElementById("anim4");
	var ctx = canvas.getContext("2d");
	function draw() {
		var w = canvas.getBoundingClientRect().width;
		var h = canvas.getBoundingClientRect().height;
		canvas.width = w;
		canvas.height = h;
		ctx.fillStyle = "rgb(10 10 10)";
		ctx.fillRect(0, 0, w, h);
		// Seconds
		var time = ((new Date()) - start) / 1000; 
	
		ctx.lineWidth = 4;

		function wave(x, t) {
			if (x >  0) return Math.sin(x * 20 + -time * 3);
			if (x <= 0) return Math.sin(x * 20 +  time * 3);
		}

		// Incedent wave
		ctx.beginPath()
		for (var i = 0; i < 256; i++) {
			var x = (i / 255);
			var phase = x * 20 + -time * 3;
			if (i > 100) phase += (Math.floor((i-100)/5)) / 4;
			var y = Math.sin(phase)/4 + .5;
			if (i == 0) ctx.moveTo(x * w, y * h);		
			else ctx.lineTo(x * w, y * h);
		}
		ctx.strokeStyle = "rgb(255 255 10)";
		ctx.stroke();
		for (var i = 100; i < 255; i += 5) {
			ctx.fillStyle = "rgb(100 100 100)";
			ctx.fillRect((i / 255) * w, 0, 5, h)
		}
		// Lables
		window.requestAnimationFrame(draw);
	}
	window.requestAnimationFrame(draw)
}
anim4()
</script>

# What happened to the reflections?

Consider the effect of two slices a quarter wavelength apart: 

<canvas id=anim5 style="width: 40em; height: 20em;">
</canvas>
<center style="color: gray">Magic.</center>

<script defer>
function anim5() {
	var canvas = document.getElementById("anim5");
	var ctx = canvas.getContext("2d");
	function draw() {
		var w = canvas.getBoundingClientRect().width;
		var h = canvas.getBoundingClientRect().height;
		canvas.width = w;
		canvas.height = h;
		ctx.fillStyle = "rgb(10 10 10)";
		ctx.fillRect(0, 0, w, h);
		// Seconds
		var time = ((new Date()) - start) / 1000; 
	
		ctx.lineWidth = 4;

		function incedent(x, t) {
			var phase = x * 20 + -t * 3;
			return Math.sin(phase);
		}

		function single(x, t) {
			t += 3.14 / 3 * 0.4;
			if (x > 100/255) {
				return -incedent(x, t) / 5;
			} else {
				return incedent(-x, t) / 5;
			}
		}

		var distance = Math.PI/9 / 4;
		function reflection(x, t) {
			if (x < 100/255) return 0;
			return single(x, t) + single(x - distance, t - 0.48)
		}

		// Incedent wave
		ctx.beginPath()
		for (var i = 0; i < 255; i++) {
			var x = (i / 255);
			var y = incedent(x, time);
			y = y / 4 + 0.5;
			ctx.lineTo(x * w, y * h);
			if (i == 100) {
				ctx.strokeStyle = "rgb(250 250 10)";
				ctx.stroke();
				ctx.beginPath()
			}
		}
		ctx.strokeStyle = "rgb(100 100 10)";
		ctx.stroke();
		// Remited wave
		var shift = Math.PI + Math.PI * 0.45
		ctx.beginPath()
		ctx.strokeStyle = "rgb(0 100 100)";
		for (var i = 0; i < 255; i++) {
			var x = (i / 255);
			var y = reflection(x, time)/4 + .5;
			ctx.lineTo(x * w, y * h);
		}
		ctx.stroke();
		// Combined wave
		ctx.beginPath()
		ctx.strokeStyle = "rgb(255 250 250)";
		for (var i = 0; i < 255; i++) {
			var x = (i / 255);
			var phase = x * 20 + -time * 3 - shift;
			var y = reflection(x, time) + incedent(x, time);
			y = y/4 + 0.5;
			ctx.lineTo(x * w, y * h);
		}
		ctx.stroke();
		// Section
		ctx.fillStyle = "rgb(100 100 100)";
		ctx.fillRect(100/255*w, 0, w/255 + 5, h)
		ctx.fillRect(100/255*w + (distance*255)*2 + 10, 0, w/255 + 5, h)
		// Lables
		ctx.font = "" + h/20 + "px sans";
		ctx.fillStyle = "rgb(255 250 0)";
		ctx.fillText("Yellow", 0, h/20);
		ctx.fillStyle = "rgb(0 250 255)";
		ctx.fillText("Cyan", 0, h/20 * 2 + 4);
		ctx.fillStyle = "rgb(250 250 255)";
		ctx.fillText("White", 0, h/20 * 3 + 8);
		ctx.fillText("Incident wave", h/5, h/20);
		ctx.fillText("Re-radiated", h/5, h/20 * 2 + 4);
		ctx.fillText("Combined", h/5, h/20 * 3 + 8);
		window.requestAnimationFrame(draw);
	}
	window.requestAnimationFrame(draw)
}
anim5()
</script>
<br>


Because reflected energy has to travel both ways, the two reflections are half a wavelength out of phase, and perfectly cancel out. 

The bulk glassl can be seen as a bunch of these slice pairs, and the only uncancled reflections are from the surface.
This matches with our everyday expereinece. The surface of glass reflects, but the rest is transparant.

# The direction of light

Leaving our comforatble one dimentional universe, this model of light has a huge hole:
Electromagnetic waves travel in all directions, but light beams don't.


# Refraction
# A lens
# Dispersion


