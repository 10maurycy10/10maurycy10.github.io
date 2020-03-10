'usestrict';
var D = function (a) {  //debug
console.log(a);
};
ui.int();
var game = {
    output : ui.outputln,
    cls : ui.cls,
    room : "street1",
    objects : { //int env
        pigon1State: 0,
        doorOpened : 0,
        consiracyDiscoverd : 0,
    },
}
var command = function(inputstr) {
    game.output(inputstr)
    if (inputstr==="over123") {
        game.cls()
        game.output("blabla")
        game.output("blabla")
        game.output("blabla")
        game.output("blabla")
        game.output("blabla")
        game.output("blabla")
        game.output("blabla")
        game.output("blabla")
        game.output("blabla")
        game.output("blabla")
        game.output("blabla")
        game.output("blabla")
        game.output("blabla")
        game.output("blabla")
        game.output("blabla")
        game.output("blabla")
        game.output("blabla")
        game.output("blabla")
        game.output("blabla")
        game.output("blabla")
        game.output("blabla")
        game.output("blabla")
        game.output("blabla")
        game.output("blabla")
        game.output("blabla")
        game.output("blabla")
        game.output("blabla")
        game.output("blabla")
        game.output("blabla")
        game.output("blabla")
        game.output("blabla")
        game.output("blabla")
        game.output("blabla")
        game.output("blabla")
        game.output("blabla")
        game.output("blabla")
        game.output("blabla")
        game.output("blabla")
        return(0)
 
    }
    if (inputstr==="inv") {
	    game.cls()
        ui.out.className = "white";
        game.output("you have:")
        if (game.objects.pigon1State===2) {
            game.output("a big gold key whith a strange symbol")
        }
        game.output("")
        game.output("(ENTER):ok")
        return(0)
    }
    if (inputstr==="help"){
	    game.cls()
	    ui.out.className = "white";
        game.output("inv(ENTER) = dispaly what you have")
        game.output("help(ENTER) = dispaly commands")
        game.output("number(ENTER) = do option n")
        game.output("")
        game.output("(ENTER):ok")
        return(0)
    }
    if (game.room==="street1") {
        D("game.room === 'street1' ok");
        game.cls()
        ui.out.className = "green";
        game.output("you are on a street")
        game.output("it is eraly quiet")
        game.output("in the distance you see feilds")
        if (game.objects.pigon1State===0){
            game.output("there is a big pigon")
        }
        if (game.objects.pigon1State===1) {
            game.output("and a gloden key on the ground")
            }
        game.output("")
        game.output("1 : go east")
        game.output("2 : go west")
        if (game.objects.pigon1State===0) {
                game.output("3 : grab the pigon")
        }
        if (game.objects.pigon1State===1) {
                game.output("3 : take the key")
        }
        if (game.objects.pigon1State===2) {
            game.output("3 : try to open doors")
        }
        if (game.objects.doorOpened===1) {
            game.output("4 : go in to the strange house")
        }
        if (inputstr==="3"){
            if (game.objects.pigon1State===0){
                game.objects.pigon1State = 1 
                game.cls()
                ui.out.className = "green";
                game.output("you chace the pigon across the street")
                game.output("but it gets away")
                game.output("you see a gold key on the groung")
                game.output("")
                game.output("(ENTER):ok")
		return(1)
            }
            if (game.objects.pigon1State===1){
                game.cls()
                ui.out.className = "green";
                game.output("you take the key")
		game.output("it has a strange simbol scratched on it")
                game.output("")
                game.output("(ENTER):ok")
		game.objects.pigon1State = 2
		return(1)
            }
            if (game.objects.pigon1State===2){
                game.cls()
                ui.out.className = "green";
                game.output("you try to open the doors to to the houses, and one opens.")
                game.output("it has the strange symbol that is on the key")
                game.output("")
                game.output("(ENTER):ok")
                game.room = "house1"
                game.objects.doorOpened = 1
                if (game.objects.doorOpened===0) {
		             command("LOOK")
                }
                return(1)
            }
        }
        if (inputstr==="1") {
            console.log("inputstr === 1")
            game.room = "fealds_1" // fields
            command("look")
	    return(0)
        }
        if (inputstr==="2") {
            game.room = "fealds_1"
            command("look")
	    return(0)
        }
	    if (inputstr==="4") {
		if (game.objects.doorOpened === 1) {
            		game.room = "house1"
            		command("look")
		}
	    	return(0)
        }
    }
    if (game.room==="fealds_1") {
        game.cls()
        ui.out.className = "green";
        game.output("you walck until you hit feilds.")
        game.output("")
        game.output("1 : go back")
        if (inputstr==="1") {
            game.room="street1"
            command("look")
	    return(0)
        }   
    }
    if (game.room==="house1") {
        game.cls()
        ui.out.className = "blue";
        game.output("in the house you see a hall way,")
        game.output("the last door seems to be unlocked")
        game.output("it seem to be empty but you thing that someone was hear a momentbe for now")
        game.output("")
        game.output("1 : go out")
        game.output("2 : open the door")
        if (inputstr==="1") {
            game.cls()
            game.room = "street1"
            game.output("you go out the front door")
            game.output("")
            game.output("(ENTER) : ok")
            return(1)
        }
        if (inputstr==="2") {
            game.cls()
            ui.out.className = "blue"
            game.room = "bacement1"
            game.output("you wack down the hallway and into the basement.")
            game.output("")
            game.output("(ENTER) : ok")
            return(1)
        }
    }
    if (game.room==="bacement1") {
        game.cls()
        ui.out.className = "gray"
        game.output("you are in the basment")
        game.output("thar is is a table whith a leter")
        game.output("")
        game.output("1 : leave")
	game.output("2 : look at the leter")
        if (inputstr==="1") {
	    game.cls()
            game.room = "house1"
            game.output("you leave the bacement")
            game.output("")
            game.output("(ENTER) : ok")
        }
	if (inputstr==="2") {
		game.cls();
		game.output("Captain,")
		game.output("Proceed with the plans as discussed. The Obstacle will be taken care of by me, personally. No need to worry.  Remember that ... (end of the parchment)");
          	game.output("")
            	game.output("(ENTER) : ok")
	}
        return(1)
    }
}
ui.command = command;
command("look")