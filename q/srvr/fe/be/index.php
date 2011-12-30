<?php

function res_list ()
{
	print json_encode(array('u' => $_SESSION['users'], 'r' => $_SESSION['res_list']));
}

function queue ($id)
{
	$_SESSION['res_list'][$id]['q'][] = $_SESSION['user_id'];
}

function dequeue ($id)
{
	$queue = &$_SESSION['res_list'][$id]['q'];

	$pos = array_search($_SESSION['user_id'], $queue);

	if ($pos !== false)
		array_splice($queue, $pos, 1);
}

switch (basename($_SERVER['SCRIPT_URL']))
{
	case 'login':

		session_start();

		if (!isset($_SERVER['PHP_AUTH_USER']))
		{
			header('WWW-Authenticate: Basic realm="QAPP"');
			header('HTTP/1.0 401 Unauthorized');
			print '401 Unauthorized.';
			die ();
		}
		else if (!isset($_SESSION['user_id']))
		{
			$_SESSION['users'] = array(
			  'u0' => 'mmarkovic'
			, 'u1' => 'ppetrovic'
			, 'u2' => 'ssimic'
			, 'u3' => 'nnikolic'
			);

			$_SESSION['user_cnt'] = 3;

			$users = &$_SESSION['users'];
			$user_cnt = &$_SESSION['user_cnt'];

			$users['u' . ++$user_cnt] = $_SERVER['PHP_AUTH_USER'];

			$_SESSION['user_id'] = 'u' . $user_cnt;

			$res_list = array();

			for ($i = 27; $i >= 0; --$i)
			{
				$queue = array();

				if (mt_rand(1, 100) <= 30)
					array_push($queue, 'u0');

				if (mt_rand(1, 100) >= 70)
					array_push($queue, 'u1');

				if (mt_rand(1, 100) <= 30)
					array_push($queue, 'u2');

				if (mt_rand(1, 100) >= 70)
					array_push($queue, 'u3');

				$res_list['r' . $i] = array(
				  'n' => 'ROOM' . $i
				, 'q' => $queue
				, 'f' => empty($queue) ? 1 : (mt_rand(1, 100) <= 50 ? 1 : 0)
				);

				shuffle($res_list['r' . $i]['q']);
			}

			$_SESSION['res_list'] = $res_list;
		}

		print 'var Q = { user_id: \'' . $_SESSION['user_id'] . '\' };';

		die ();
		break;

	case 'list':

		session_start();

		res_list();

		die ();
		break;

	case 'queue':

		session_start();

		queue(file_get_contents('php://input', FILE_BINARY));

		res_list();

		die ();
		break;

	case 'dequeue':

		session_start();

		dequeue(file_get_contents('php://input', FILE_BINARY));

		res_list();

		die ();
		break;
}

?>