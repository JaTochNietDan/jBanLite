<?php

include('config.php');
include('includes/functions.php');
include('includes/header.php');

if(empty($_GET['orderby'])) $order_index = 0;
else if($_GET['orderby'] > 0 && $_GET['orderby'] < 6 && is_numeric($_GET['orderby'])) $order_index = $_GET['orderby'];
else $order_index = 0;

if(empty($_GET['order'])) $order_dir = 'DESC';
else if($_GET['order'] == 'ASC') $order_dir = 'ASC';
else $order_dir = 'DESC';

if(empty($_GET['show'])) 
{
	$page = 'active';
	$result = $SQL_DB->query('SELECT *, (strftime(\'%s\', datetime(ban_timestamp, \'+\' || ban_time || \' minute\')) - (strftime(\'%s\', CURRENT_TIMESTAMP))) AS ban_timeleft FROM `'.$SQL_Table.'` WHERE ban_timeleft > 0 OR ban_time = 0 ORDER BY '.$OrderReplace[$order_index].' '.$order_dir);
	echo 'Showing: Active | <a href="?show=expired">Expired</a> | <a href="?show=all">All</a>';
}
else if($_GET['show'] == "expired") 
{
	$page = 'expired';
	$result = $SQL_DB->query('SELECT *, (strftime(\'%s\', datetime(ban_timestamp, \'+\' || ban_time || \' minute\')) - (strftime(\'%s\', CURRENT_TIMESTAMP))) AS ban_timeleft FROM `'.$SQL_Table.'` WHERE ban_timeleft < 0 AND ban_time != 0 ORDER BY '.$OrderReplace[$order_index].' '.$order_dir);
	echo 'Showing: <a href="?show=active">Active</a> | Expired | <a href="?show=all">All</a>';
}
else if($_GET['show'] == "all") 
{
	$page = 'all';
	$result = $SQL_DB->query('SELECT *, (strftime(\'%s\', datetime(ban_timestamp, \'+\' || ban_time || \' minute\')) - (strftime(\'%s\', CURRENT_TIMESTAMP))) AS ban_timeleft FROM `'.$SQL_Table.'` ORDER BY '.$OrderReplace[$order_index].' '.$order_dir);
	echo 'Showing: <a href="?show=active">Active</a> | <a href="?show=expired">Expired</a> | All';
}
else
{
	$page = 'active';
	$result = $SQL_DB->query('SELECT *, (strftime(\'%s\', datetime(ban_timestamp, \'+\' || ban_time || \' minute\')) - (strftime(\'%s\', CURRENT_TIMESTAMP))) AS ban_timeleft FROM `'.$SQL_Table.'` WHERE ban_timeleft > 0 OR ban_time = 0 ORDER BY '.$OrderReplace[$order_index].' '.$order_dir);
	echo 'Showing: Active | <a href="?show=expired">Expired</a> | <a href="?show=all">All</a>';
}

?>



<table width="70%">
	<tr>
		<th><a class="title" href="<?php echo '?show='.$page.($order_dir == 'ASC' && $order_index == 1 ? '' : '&order=ASC'); ?>&orderby=1">Player</a></th>
		<th><a class="title" href="<?php echo '?show='.$page.($order_dir == 'ASC' && $order_index == 2 ? '' : '&order=ASC'); ?>&orderby=2">Admin</a></th>
		<th><a class="title" href="<?php echo '?show='.$page.($order_dir == 'ASC' && $order_index == 3 ? '' : '&order=ASC'); ?>&orderby=3">Ban Reason</a></th>
		<th><a class="title" href="<?php echo '?show='.$page.($order_dir == 'ASC' && $order_index == 4 ? '' : '&order=ASC'); ?>&orderby=4">Time</a></th>
		<th><a class="title" href="<?php echo '?show='.$page.($order_dir == 'ASC' && $order_index == 5 ? '' : '&order=ASC'); ?>&orderby=5">Time Left</a></th>
	</tr>
	<?php
	foreach($result as $row)
	{
		$seconds = $row['ban_timeleft'];
		
		$days = floor($seconds / (60 * 60 * 24));
		
		$divisor_for_hours = $seconds % (60 * 60 * 24);
		$hours = floor($divisor_for_hours / (60 * 60));

		$divisor_for_minutes = $divisor_for_hours % (60 * 60);
		$minutes = floor($divisor_for_minutes / 60);
	 
		$divisor_for_seconds = $divisor_for_minutes % 60;
		$seconds = ceil($divisor_for_seconds);	
		
		echo '<tr><td>'.$row['user_banned'].'</td><td>'.$row['user_banner'].'</td><td>'.$row['ban_reason'].'</td><td>'.$row['ban_timestamp'].'</td><td>'.($row['ban_time'] == 0 ? 'Permanent' : ($row['ban_timeleft'] < 0 ? "Expired" : $days.'days '.$hours.'hrs '.$minutes.'mins '.$seconds.'secs')).'</td></tr>';
	}
	?>
</table>

<?php
include('includes/footer.php');

?>