<?php

function queue ($id)
{
	$_SESSION['res_list'][$id]['q'][] = $_SESSION['username'];
}

function dequeue ($id)
{
	$queue = &$_SESSION['res_list'][$id]['q'];

	$pos = array_search($_SESSION['username'], $queue);

	if ($pos !== false)
		array_splice($queue, $pos, 1);
}

switch ($_SERVER['QUERY_STRING'])
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
		else if (!isset($_SESSION['username']))
		{
			$username = $_SERVER['PHP_AUTH_USER'];

			$_SESSION['username'] = $username;

			$res_list = array();

			for ($i = 1; $i <= 28; ++$i)
			{
				$queue = array();

				if (mt_rand(1, 100) <= 30)
					array_push($queue, 'mmarkovic');

				if (mt_rand(1, 100) >= 70)
					array_push($queue, 'ppetrovic');

				if (mt_rand(1, 100) <= 30)
					array_push($queue, 'ssimic');

				if (mt_rand(1, 100) >= 70)
					array_push($queue, 'nnikolic');

				$res_list['r' . $i] = array(
				  'n' => 'ROOM' . $i
				, 'q' => $queue
				, 'f' => empty($queue) ? true : mt_rand(1, 100) <= 50
				);

				shuffle($res_list['r' . $i]['q']);
			}

			$_SESSION['res_list'] = $res_list;
		}

		print 'var Q = { username: ' . json_encode($_SESSION['username']) . ' };';

		die ();
		break;

	case 'list':

		session_start();

		print json_encode($_SESSION['res_list']);

		die ();
		break;

	case 'queue':

		session_start();

		queue(file_get_contents('php://input', FILE_BINARY));

		print json_encode($_SESSION['res_list']);

		die ();
		break;

	case 'dequeue':

		session_start();

		dequeue(file_get_contents('php://input', FILE_BINARY));

		print json_encode($_SESSION['res_list']);

		die ();
		break;
}

?>