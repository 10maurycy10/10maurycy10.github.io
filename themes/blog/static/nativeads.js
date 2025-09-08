function hide() {
        document.getElementById("ad-note").id = 'ad-note-hidden'
        document.cookie = "notice-shown=true;path=/"
}

// Show message
if (document.cookie.includes("notice-shown")) {
	console.log("User dismissed message")
} else {
	document.getElementById("ad-note-hidden").id = 'ad-note'
	document.getElementById("ad-note-content-wrapper").innerHTML = "No adblocker detected. " + 
	"Consider using an extention like <a href=https://ublockorigin.com/>uBlock Origin</a> to save time and bandwidth." +
	 " <u onclick=hide()>Click here to close.</u>"
	
}

