var ui = {
    int: function (){
        ui.input = document.getElementById("inputtag");    //int io
        ui.out = document.getElementById("outputTag");},
    outputln: (function(a){
        ui.out.innerHTML = ui.out.innerHTML + a + "<br>";}),
    cls: function () {
        ui.out.innerHTML = "";
    },
    keypress : function(){
        if (event.keyCode===13) {
            ui.command(ui.input.value)
            ui.input.value = ""
        }
    },
    command : function () {},
}
