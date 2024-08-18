


function saveMemo() {
    var subject =  document.querySelector("#subject");
    var memo =  document.querySelector("#memo");
    var ido;
    var keido;

    if (subject.validity.valid == false) {
        alert("件名を入れてください。")
        subject.focus();
        return;
    }
    if (subject.validity.valid == false) {
        alert("メモを入れてください。")
        memo.focus();
        return;
    }
    if (sessionStorage) {
        ido = sessionStorage.getItem("latitude");
        keido = sessionStorage.getItem("longitude");
    }
    var memo_list = [];
    if (localStorage) {
        var memo_item = localStorage.getItem("位置情報メモ");
        if (memo_item) {
            memo_list = JSON.parse(memo_item);
        }
        memo_list.push({
            "緯度": ido,
            "経度": keido,
            "件名": subject.value,
            "メモ": memo.value
        });
        memo_item = JSON.stringify(memo_list, null, 2);
        localStorage.setItem("位置情報メモ", memo_item);
        console.log(memo_item);
        // alert(memo_item);
    }
    location.href="lesson03.html";
}
function update_json() {
    console.log("update_json");
    var jsonstorage = document.querySelector("#jsonstorage");
    let strjson = jsonstorage.value;
    let data_json = JSON.parse(strjson);
    strjson = JSON.stringify(data_json);
    if (localStorage) {
        localStorage.setItem("位置情報メモ", strjson);
    }
}


window.onload = function() {
    // var input_form = document.querySelector("#input_form");
    var save = document.querySelector("#save");
    var cancel = document.querySelector("#cancel");
    // var subject =  document.querySelector("#subject");
    // var memo =  document.querySelector("#memo");
    let updatejson = document.querySelector("#updatejson");
    var jsonstorage = document.querySelector("#jsonstorage");
    cancel.onclick = function() {
        location.href="lesson03.html";
    }
    updatejson.onclick=update_json;
    if (localStorage) {
        let item = localStorage.getItem("位置情報メモ");
        let json_item =[];
        if (item) {
            json_item = JSON.parse(item);
        }
        jsonstorage.value = JSON.stringify(json_item, null, 1);
    }
    save.addEventListener("click", function(e){
        e.preventDefault();
        saveMemo();
    }, false);
}

