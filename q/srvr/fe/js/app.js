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
				qpos = queue.indexOf(Q.username),
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
			res.push(' - ', queue[0]);

		res.push('<br />');

		res.push('<div id="res_', item.i, '_msg" class="res_msg">', msg, '</div>');

		if (qlen - (item.f ? 0 : 1))
			res.push('<div id="res_', item.i, '_queue" class="res_queue">', queue.slice(item.f ? 0 : 1).join(', '), '</div>');

		res.push('</div>');

		res = $(res.join(''));

		if (flash)
			Q.flash_map[id] = res;

		res.click({ qpos: qpos, id: id }, function (ev)
		{
			$.ajax({
			  url: ev.data.qpos != -1 ? '/be/?dequeue' : '/be/?queue'
			, type: 'POST'
			, data: ev.data.id
			, dataType: 'json'
			, success: function (list)
				{
					Q.res_show(list);
				}
			});
		});

		res.hover(function () { $(this).css('cursor', 'pointer'); }, function () { $(this).css('cursor', ''); });

		res_cntnr.append(res);
	}
};

Q.run = function ()
{
	$.ajax({
	  url: '/be/?list'
	, dataType: 'json'
	, success: function (list)
		{
			Q.res_show(list);
		}
	});

};

setInterval(Q.flash, 200);