


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
        memo_item = JSON.stringify(memo_list);
        localStorage.setItem("位置情報メモ", memo_item);
        console.log(memo_item);
        alert(memo_item);
    }
    location.href="lesson03.html";
}

window.onload = function() {
    var input_form = document.querySelector("#input_form");
    var save = document.querySelector("#save");
    var cancel = document.querySelector("#cancel");
    var subject =  document.querySelector("#subject");
    var memo =  document.querySelector("#memo");

    if (navigator.geolocation) {
        console.log("位置情報取得可能")            
    } else {
        console.log("位置情報取得できません。")            
    }
    cancel.onclick = function() {
        location.href="lesson03.html";
    }

    save.addEventListener("click", function(e){
        e.preventDefault();
        saveMemo();
    }, false);
}

