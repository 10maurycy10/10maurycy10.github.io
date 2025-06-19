// Show message
if (document.cookie.includes("notice-shown")) {
	console.log("User dismissed message")
} else {
	document.getElementById("ad-note-hidden").id = 'ad-note'
}

