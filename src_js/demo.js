let SPASSWrapper = require('./SPASSWrapper.js');
window.addEventListener('load', windowReady, false);

function windowReady() {
    console.log('window ready');
    SPASSWrapper.setWasmPath("./wasm/SPASS.wasm");
    document.getElementById('btn_mem').onclick = btn_mem_onClick;
    document.getElementById('btn_play').onclick = btn_play_onClick;
}

let autorun = false;
let intervalRef = undefined;
function btn_mem_onClick() {
    autorun = !autorun;
    if (autorun) {
        document.getElementById('btn_mem').innerText = "Stoooop!";
        intervalRef = window.setInterval(doAutoRun, 100);
    }else{
        if(intervalRef){
            document.getElementById('btn_mem').innerText = "RUN RUN RUN!";
            window.clearInterval(intervalRef);
        }
    }
}

let text = undefined;
function doAutoRun() {
    if (intervalRef) {
        window.clearInterval(intervalRef);
    }
    let r = Promise.resolve();
    if(!text){
        r = r.then(_=>{
            return fetch("/problems/sokrates.problem");
        }).then(response=>{
            return response.text();
        }).then(value=>{
            text = value;
        });
    }
    r.then(_=>{
        return run(text);
    }).then(result => {
        if(result.indexOf("SPASS beiseite: Proof found.")==-1){
            console.warn("SPASS seems to not work properly...");
        }
        if (autorun) {
            intervalRef = window.setInterval(doAutoRun, 100);
        }
    }).catch(reason=>{
        console.log("spass failed. ", reason);
    })
}


function btn_play_onClick() {
    document.getElementById("btn_play").setAttribute("disabled", true);
    let args = document.getElementById('inpt_commandline').value.split(" ");
    if(args.length == 1 && args[0] ==""){
        args = [];
    }
    let data = document.getElementById('inpt_input').value;
    console.log("args: ", args);
    console.log("data: ", data);
    run(data, args).then(value => {
        document.getElementById('inpt_output').value = value;
        document.getElementById("btn_play").removeAttribute("disabled");
    }).catch(reason=>{
        console.log("spass failed. ", reason);
    });
}

let SPASScount = 0;
function run(data, options) {
    SPASScount++;
    document.getElementById('spn_counter').innerText = SPASScount;
    return SPASSWrapper.run(data, options);
}