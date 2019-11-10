const SPASS = require("../release/SPASSWrapper.js");

SPASS.setWASMPATH("./SPASS.wasm");

document.getElementById("run").addEventListener("click", () => {
	document.getElementById("run").disabled = true;

	let input = document.getElementById("input").value;

	let args = document.getElementById("arguments").value.split(" ");
	if (args.length === 1 && args[0] === "") {
		args = [];
	}

	SPASS.run(input, args).then(output => {
		document.getElementById("output").value = output;
	}).catch(reason => {
		document.getElementById("output").value = `Execution failed: \n\n${reason}`;
	}).finally(() => {
		document.getElementById("run").disabled = false;
	});
});
