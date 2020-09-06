var DOM_F_REG,
	DOM_F_CAP;

function myFetch(type) {
	return fetch("bbsform?type=" + type)
		.then(response => response.json());
}

function switch_view(status) {
	// status == 0, CAPTCHA_OK
	if (status == 0) {
		DOM_F_REG.classList.add("hidden");
		DOM_F_CAP.classList.remove("hidden");
	} else {
		DOM_F_REG.classList.remove("hidden");
		DOM_F_CAP.classList.add("hidden");
	}
}

function update_form_email() {
	var email = document.querySelector("#cap-email").innerText, arr, idx;
	if (email.length > 0) {
		arr = email.split("@");

		if (arr.length == 2) {
			document.querySelector("input[name=user]").value = arr[0];

			idx = 1;
			if (arr[1] == "xjtu.edu.cn") idx = 1;
			else if (arr[1] == "stu.xjtu.edu.cn") idx = 2;
			else if (arr[1] == "mail.xjtu.edu.cn") idx = 3;
			document.querySelector("select[name=popserver]").value = idx;
		}
	}
}

function get_status(status) {
	let msg;
	switch (status) {
	case -1: msg = "��Ǹ����ϵͳ���ϣ�����ϵվ��"; break;
	case -3: msg = "������û����֤�룬�������ύ��"; break;
	case 1: msg = "������֤����ʧЧ"; break;
	case 3: msg = "������֤���ѳ�ʱ���������ύ��"; break;
	default : msg = "";
	}
	return msg;
}

document.addEventListener("DOMContentLoaded", function() {
	DOM_F_REG = document.querySelector("#form-reg");
	DOM_F_CAP = document.querySelector("#form-cap");

	var form = document.querySelector("#form-reg form"),
		modal = document.querySelector("#myModal"),
		span = document.querySelector(".close"),
		p = document.querySelector("#myModal p");

	switch_view(cap_status);
	update_form_email();

	span.addEventListener("click", function() {
		modal.style.display = "none";
	});

	window.addEventListener("click", function(event) {
		if (event.target == modal) {
			modal.style.display = "none";
		}
	});

	document.querySelector("#btn-reg-submit").addEventListener("click", function() {
		modal.style.display = "block";
		p.innerText = "������֤��Ϣ���ڱ��ύ�������ĵȴ�...";
		form.submit();
	});

	document.querySelector("#btn-reg-reset").addEventListener("click", function() {
		form.reset();
		update_form_email();
	});

	document.querySelector("#btn-cap-submit").addEventListener("click", function() {
		modal.style.display = "block";
		p.innerText = "����У��������Ϣ�������ĵȴ�...";

		myFetch("2&code=" + document.querySelector("input[name=captcha]").value).then(function(data) {
			modal.style.display = "block";
			if (data.status == 0) {
				p.innerText = "���ѳɹ�ͨ����֤����ˢ����ҳ������������";
			} else {
				p.innerText = get_status(data.status);
			}
		});
	});

	document.querySelector("#btn-cap-resent").addEventListener("click", function() {
		modal.style.display = "block";
		p.innerText = "���������ѷ��ͣ������ĵȴ�...";

		myFetch("3").then(function(data) {
			modal.style.display = "block";
			if (data.status == 1) {
				// �ѷ���
				p.innerText = "��������䣬���� 15 ������������֤��";
			} else {
				p.innerText = "�޷���֤�������䣬������֤��Ϣ";
			}
		});
	});

	document.querySelector("#btn-reg-captcha").addEventListener("click", function() {
		switch_view(0);
	});

	document.querySelector("#btn-cap-update").addEventListener("click", function() {
		switch_view(1);
	});
});

