document.addEventListener("DOMContentLoaded", function(event) {    
    document.getElementById("modals").addEventListener("click", function(event) {
        console.log(event);
        if(event.target.id == "modals"){
            modals.style.display = "none";
        }
    });
    document.getElementById("image-url-btn").addEventListener("click", function(event) {
        var modals = document.getElementById("modals");
        var index = modals.dataset.index;
        modals.style.display = "none";

        var value = document.getElementById("url").value;
        document.getElementById("url").value = "";

        var hexValue = toHex(value);
        if(hexValue.length >= 5){
            quill.insertEmbed(index, "image", "/proxy/" + toHex(value));
        }
    });

    var quill = new Quill("#editor", {
        theme: "snow",
        modules: {
            toolbar: [
                [{ header: [1, 2, false] }],
                ["bold", "italic", "underline"],
                ["image"]
            ]
        },
    });

    function imageHandler(data) {
        var modals = document.getElementById("modals");
        modals.dataset.index = quill.getSelection().index;
        modals.style.display = "block";
    }

    var toolbar = quill.getModule("toolbar");
    toolbar.addHandler("image", imageHandler);

    function toHex(str) {
        var hex = "";
        for (var i = 0; i < str.length; i++) {
            hex += str.charCodeAt(i).toString(16);
        }
        return hex;
    };
});
