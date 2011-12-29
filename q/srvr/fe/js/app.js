if (typeof Q != 'object')
	var Q = {};

Q.flash_map = {};
Q.flash_flag = false;

Q.flash = function ()
{
	var map = Q.flash_map;

	if (Q.flash_flag)
		for (var id in map)
			map[id].css('backgroundColor', '');
	else
		for (var id in map)
			map[id].css('backgroundColor', '#ffff88');

	Q.flash_flag = !Q.flash_flag;
};

Q.res_show = function (list)
{
	var res_cntnr = $('#res_cntnr');

	res_cntnr.empty()
	Q.flash_map = {};

	for (var id in list)
	{
		var item = list[id],
				queue = item.q,
				qlen = queue.length,
				qpos = queue.indexOf(Q.user_id),
				flash = false,
				msg;

		var res = [ '<div id="res_', item.i, '" class="res res_' ];

		if (!qlen)
		{
			res.push('free');
			msg = '';
		}
		else if (qpos != 0)
		{
			res.push('occu');
			msg = qpos != -1 ? '<span class="res_curr">&nbsp;&nbsp;<blink>WAITING ('.concat(qpos, ')</blink>&nbsp;&nbsp;</span>') : '';
		}
		else if (item.f)
		{
			res.push('free');
			msg = 'GO!!!';
			flash = true;
		}
		else
		{
			res.push('curr');
			msg = '';
		}

		res.push('">', item.n);

		if (!item.f)
			res.push(' - ', Q.user_map[queue[0]]);

		res.push('<br />');

		res.push('<div id="res_', item.i, '_msg" class="res_msg">', msg, '</div>');

		var qstart = item.f ? 0 : 1;

		if (qlen - qstart)
		{
			res.push('<div id="res_', item.i, '_queue" class="res_queue">');

			res.push(Q.user_map[queue[qstart]]);

			for (var i = qstart + 1; i < qlen; ++i)
				res.push(', ', Q.user_map[queue[i]]);

			res.push('</div>');
		}

		res.push('</div>');

		res = $(res.join(''));

		if (flash)
			Q.flash_map[id] = res;

		res.click({ qpos: qpos, id: id }, function (ev)
		{
			clearTimeout(Q.refresh_tid);

			$.ajax({
			  url: ev.data.qpos != -1 ? '/be/?dequeue' : '/be/?queue'
			, type: 'POST'
			, data: ev.data.id
			, dataType: 'json'
			, success: function (data)
				{
					Q.user_map = data.u;
					Q.res_show(data.r);
				}
			});
		});

		res.hover(function () { $(this).css('cursor', 'pointer'); }, function () { $(this).css('cursor', ''); });

		res_cntnr.append(res);
	}

	Q.refresh_tid = setTimeout(Q.refresh, 3000);
};

Q.refresh = function ()
{
	$.ajax({
	  url: '/be/?list'
	, dataType: 'json'
	, success: function (data)
		{
			Q.user_map = data.u;
			Q.res_show(data.r);
		}
	});
};

setInterval(Q.flash, 300);