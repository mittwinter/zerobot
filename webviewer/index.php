<?php
$path = '/path/to/zerobot.sqlite';
$network = 'Freenode';
$channel = '#zerobot';
$linesPerPage = 100;

class IRCLogSQLite {
	protected $channel;
	protected $baseQueryString;

	protected $db;

	public function __construct( $channel, $filename, $flags = NULL ) {
		$this->channel = $channel;
		if( !isset( $flags ) ) {
			$flags = SQLITE3_OPEN_READONLY;
		}
		$this->open( $filename, $flags );
		$this->baseQueryString = '
			SELECT *
			FROM ( SELECT "privmsg" AS type, timestamp, "<" || nick || "> " || message AS line
			       FROM log_privmsg
			       WHERE channel = "' . $this->db->escapeString( $this->channel ) . '"
			       UNION ALL
			       SELECT "names" AS type, timestamp, "Users in channel: " || namesList AS line
			       FROM log_names
			       WHERE channel = "' . $this->db->escapeString( $this->channel ) . '"
			       UNION ALL
			       SELECT "nick" AS type, timestamp, oldNick || " changed his nick to " || newNick || "." AS line
			       FROM log_nick
			       WHERE channel = "' . $this->db->escapeString( $this->channel ) . '"
			       UNION ALL
			       SELECT "join" AS type, timestamp, nick || " has joined." AS line
			       FROM log_join
			       WHERE channel = "' . $this->db->escapeString( $this->channel ) . '"
			       UNION ALL
			       SELECT "quit" AS type, timestamp, nick || " has left (" || quitMessage || ")" AS line
			       FROM log_quit
			       WHERE channel = "' . $this->db->escapeString( $this->channel ) . '"
			     )
			';
	}

	function __destruct() {
		$this->close();
	}

	function open( $filename, $flags ) {
		$this->checkPermissions( $filename, $flags );
		$this->db = new SQLite3( $filename, $flags );
	}

	public function close() {
		$this->db->close();
	}

	public function getLogLineCount( $timestampBegin, $timestampEnd ) {
		$rs = $this->db->query( '
				SELECT COUNT(*) AS count
				FROM ( ' . $this->baseQueryString . '
				       WHERE timestamp >= ' . $this->db->escapeString( $timestampBegin ) . ' AND timestamp <= ' . $this->db->escapeString( $timestampEnd ) . '
				     )
				' );
		if($rs === false) {
			throw new Exception( 'SQLite error #' . $this->db->lastErrorCode() . ': ' . $this->db->lastErrorMsg() . '.' );
		}
		$logCount = $rs->fetchArray();
		return intval( $logCount[ 'count' ] );
	}

	public function getLogFormatted( $timestampBegin, $timestampEnd, $offset, $count ) {
		$rs = $this->db->query(
				$this->baseQueryString . '
				WHERE timestamp >= ' . $this->db->escapeString( $timestampBegin ) . ' AND timestamp <= ' . $this->db->escapeString( $timestampEnd ) . '
				ORDER BY timestamp
				LIMIT ' . $this->db->escapeString( $offset ) . ',' . $this->db->escapeString( $count ) . '
				;
			' );
		if($rs === false) {
			throw new Exception( 'SQLite error #' . $this->db->lastErrorCode() . ': ' . $this->db->lastErrorMsg() . '.' );
		}
		$formattedLog = '';
		$logLine = false;
		while( ($logLine = $rs->fetchArray()) !== false) {
			$logLine[ 'line' ] = preg_replace( '/^<(.+?)> ACTION(.*)$/', '* $1 $2',$logLine[ 'line' ] );
			$logLine[ 'line' ] = preg_replace( '/[0-9]{0,2}/', '', $logLine[ 'line' ] );
			$logLine[ 'line' ] = preg_replace( '//',           '', $logLine[ 'line' ] );
			$logLine[ 'line' ] = preg_replace( '//',           '', $logLine[ 'line' ] );
			$logLine[ 'line' ] = iconv( mb_detect_encoding( $logLine[ 'line' ], array( 'UTF-8' , 'ISO-8859-1', 'WINDOWS-1252' ) ), 'utf-8', $logLine[ 'line' ] );
			$logLine[ 'line' ] = xmlentities( $logLine[ 'line' ] );
			$logLine[ 'line' ] = preg_replace( '/([a-zA-Z]{3,5}:\/\/[a-zA-Z0-9-_\.:@#]+(?:(\/\S+)+)?\/?)/u', '<a href="$1" title="Jump to link destination" target="_blank">$1</a>', $logLine[ 'line' ] );
			//$logLine[ 'line' ] = preg_replace( '/\/(...*?)\//', '<em>$1</em>', $logLine[ 'line' ] );
			//$logLine[ 'line' ] = preg_replace( '/_(...*?)_/', '<span style="text-decoration: underline;">$1</span>', $logLine[ 'line' ] );
			if( $logLine[ 'type' ] == 'names' || $logLine[ 'type' ] == 'nick' || $logLine[ 'type' ] == 'join' || $logLine[ 'type' ] == 'quit') {
				$logLine[ 'line' ] = '<span class="system">! ' . $logLine['line'] . '</span>';
			}
			if( $logLine[ 'type' ] == 'privmsg' ) {
				$matches = array();
				if( preg_match( '/^&#60;(.+?)&#62; ([^ ]+?)[:,] /u', $logLine[ 'line' ], $matches ) > 0 ) {
					if( strlen( $matches[ 2 ] ) > 1 && !in_array( $matches[ 1 ], array( 'Title' ) ) ) {
						$color = getNickColor( $matches[ 2 ] );
						$logLine[ 'line' ] = preg_replace( '/^(&#60;.+?&#62;) ([^ ]+?)([:,] )/u', '$1 <span style="color: ' . $color . ';">$2</span>$3', $logLine[ 'line' ] );
					}
				}
				if( preg_match( '/^&#60;(.+?)&#62;/u', $logLine[ 'line' ], $matches ) > 0 ) {
					$color = getNickColor( $matches[ 1 ] );
					$logLine[ 'line' ] = preg_replace( '/^&#60;(.+?)&#62;/u', '&#60;<span style="color: ' . $color . ';">$1</span>&#62;', $logLine[ 'line' ] );
				}
				$logLine[ 'line' ] = preg_replace( '/^(&#60;.+?&#62;)/u', '<span class="nick">$1</span>', $logLine[ 'line' ] );
				if( preg_match( '/^\* (.+?) /u', $logLine[ 'line' ], $matches ) > 0 ) {
					$color = getNickColor( $matches[ 1 ] );
					$logLine[ 'line' ] = preg_replace( '/^\* (.+?) /u', '* <span class="nick"><span style="color: ' . $color . ';">$1</span></span> ', $logLine[ 'line' ] );
				}
			}
			$formattedLog .= '<span class="time">[' . strftime( '%Y-%m-%d %H:%M:%S', $logLine[ 'timestamp' ] ) . ']</span> ' . $logLine[ 'line' ] . '<br />' . PHP_EOL;
		}
		return $formattedLog;
	}

	private function checkPermissions( $filename, $flags ) {
		if( !file_exists( $filename) && ($flags & SQLITE3_OPEN_CREATE) && !is_writable( dirname( $filename ) ) ) {
			throw new Exception('Cannot create database '.$filename.' as requested by flag SQLITE3_OPEN_CREATE.');
		}
		else if( ($flags & SQLITE3_OPEN_READWRITE) && !is_writable( $filename) ) {
			throw new Exception('Cannot write to database '.$_filename.' as requested by flag SQLITE3_OPEN_READWRITE.');
		}
		else if( !is_readable( $filename ) ) {
			throw new Exception('Cannot read database '.$_filename.'.');
		}
	}
}

function getNickColor( $nick ) {
	$colors = array(
		  '#DC143C'
		, '#00008B'
		, '#006400'
		, '#8B008B'
		, '#8B0000'
		, '#483D8B'
		, '#2F4F4F'
		, '#B22222'
		, '#4B0082' // indigo raus?
		, '#C71585'
		, '#FF4500'
		, '#CD853F'
		, '#A0522D' // sienna
		, '#A52A2A'
		, '#D2691E'
		);
	$hashedNick = hash('sha256', $nick );
	$colorHash = 0;
	for( $i = 0; $i < strlen( $hashedNick ); $i++ ) {
		$colorHash += ord( $hashedNick[ $i ] ) % count( $colors );
	}
	$colorHash %= count( $colors );
	return $colors[ $colorHash ];
}

function xmlentities( $_string, $_quote_style = ENT_COMPAT ) {
	static $trans;
	if( !isset( $trans ) || empty( $trans ) ) {
		$html_trans = get_html_translation_table( HTML_ENTITIES, $_quote_style );
		while( list( $chr, ) = each( $html_trans ) ) {
			$trans[ utf8_encode( $chr ) ] = '&#' . ord( $chr ).';';
		}
		// dont translate the '&', we'll look at this case later more closely
		$trans[ utf8_encode( chr( 38 ) ) ] = '&';
	}

	$_string = strtr( $_string, $trans );
	// after the initial translation, _do_ map standalone '&' into '&#38;'
	$_string = preg_replace( '/&(?!([A-Za-z]{0,5};|#[0-9]{2,5};))/u', '&#38;' , $_string );

	return $_string;
}

function selfURL() {
	$url = 'http';
	if( isset( $_SERVER[ 'HTTPS' ] ) && $_SERVER[ 'HTTPS' ] == 'on' ) {
		$pageURL .= 's';
	}
	$url .= '://';
	$url .= $_SERVER[ 'SERVER_NAME' ];
	if( isset( $_SERVER[ 'SERVER_PORT' ] ) && $_SERVER[ 'SERVER_PORT' ] != '80' ) {
		$url .= ':' . $_SERVER[ 'SERVER_PORT' ];
	}
	$url .= $_SERVER["REQUEST_URI"];
	return $url;
}

// die('<h1>404 Not Found</h1>');

$logDatabase = new IRCLogSQLite( $channel, $path );
// Cache certain values for view action mode:
if( isset( $_GET[ 'action' ] ) && $_GET[ 'action' ] == 'view' && isset( $_GET[ 'date' ] ) ) {
	$logLines = $logDatabase->getLogLineCount( strtotime( $_GET[ 'date' ] ), (strtotime( $_GET[ 'date' ] ) + 24 * 3600)   );
}

if( isset( $_GET[ 'action' ] ) && $_GET[ 'action' ] == 'latest' ) {
	header( '303 See Other' );
	header( 'Location: ?action=view&date=' .  strftime( '%Y-%m-%d' ) . '&page=' . intval( $logDatabase->getLogLineCount( mktime( 0, 0, 0 ), mktime( 0, 0, 0 ) + 24 * 3600 ) / $linesPerPage ) . '#bottom' );
	exit;
}
else if( isset( $_GET[ 'action' ] ) && $_GET[ 'action' ] == 'view' && isset( $_GET[ 'date' ] ) ) {
	if( strtotime( $_GET[ 'date' ] ) < (mktime( 0, 0, 0 ) - 7 * 24 * 3600) ) {
		header('404 Not Found');
		die('<h1>404 Not Found</h1>');
	}
}

header('Content-Type: text/html; charset=UTF-8');
echo( '<?xml version="1.0" encoding="utf-8" ?>' );
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
	<head>
		<link rel="stylesheet" href="log.css" type="text/css" />
		<title><?php echo( $network . ', ' . $channel ); ?></title>
		<script type="text/javascript">
		function bookmark(url, title) {
			var url = url || location.href;
			var title = title || document.title;
			// Internet Explorer:
			if(document.all) {
				window.external.AddFavorite(url, title);
			}
			// Firefox:
			else if(window.sidebar) {
				window.sidebar.addPanel(title, url, '');
			}
			return false;
		}
		</script>
	</head>

	<body>
		<h2><?php echo( 'Logs for ' . $channel . ' on ' . $network ); ?></h2>
		<h4>
			<?php
			if( isset( $_GET[ 'action' ] ) && $_GET[ 'action' ] == 'view' ) {
				if( isset( $_GET[ 'page' ] ) ) {
					$offset = intval( $_GET[ 'page' ] ) * $linesPerPage;
					$displayedLines = (($logLines - $offset) > $linesPerPage ? $linesPerPage : $logLines % $linesPerPage);
				}
				else {
					$offset = 0;
					$displayedLines = ($logLines > $linesPerPage ? $linesPerPage : $logLines);
				}
				echo( 'From ' . $_GET[ 'date' ] . ', lines ' . $offset . ' - ' . ($offset + $displayedLines) );
			}
			?>
		</h4>
		<?php
		if( isset( $_GET[ 'action' ] ) && $_GET[ 'action' ] == 'view' && isset( $_GET[ 'date' ] ) ):
		?>
		<div class="navbar">
			<p class="left">
				<a href="?" title="Go back">back</a>
				Pages:
				<?php
				$lineCount = $logDatabase->getLogLineCount( strtotime( $_GET[ 'date' ] ), (strtotime( $_GET[ 'date' ] ) + 24 * 3600) );
				for( $i = 0; $i <= ($lineCount / $linesPerPage); $i++ ) {
					if( ( !isset( $_GET[ 'page'] ) && $i != 0 ) || (isset( $_GET[ 'page' ] ) && $i != intval( $_GET[ 'page' ] ) ) ) {
						echo( '<a href="?action=view&#38;date=' . urlencode( $_GET[ 'date' ] ) . '&#38;page=' . urlencode( $i ) . '" title="Go to page ' . $i . '">' . $i . '</a>' );
					}
					else {
						echo( '<strong>' . $i . '</strong>' );
					}
					echo( ' ' );
				}
				?>
			</p>
			<p class="right">
				<a href="#bottom" title="Jump to bottom">&#8595; bottom &#8595;</a>
			</p>
			<div style="clear: both;"></div>
		</div>
		<p id="log">
			<?php
			echo( $logDatabase->getLogFormatted( strtotime( $_GET[ 'date' ] ), (strtotime( $_GET[ 'date' ] ) + 24 * 3600), (isset( $_GET[ 'page' ] ) ? $_GET[ 'page' ] * $linesPerPage : 0), $linesPerPage ) );
			?>
		</p>
		<a name="bottom" href="bottom"></a>
		<div class="navbar">
			<p class="left">
				<a href="?" title="Go back">back</a>
				Pages:
				<?php
				$lineCount = $logDatabase->getLogLineCount( strtotime( $_GET[ 'date' ] ), (strtotime( $_GET[ 'date' ] ) + 24 * 3600) );
				for( $i = 0; $i <= ($lineCount / $linesPerPage); $i++ ) {
					if( ( !isset( $_GET[ 'page'] ) && $i != 0 ) || (isset( $_GET[ 'page' ] ) && $i != intval( $_GET[ 'page' ] ) ) ) {
						echo( '<a href="?action=view&#38;date=' . urlencode( $_GET[ 'date' ] ) . '&#38;page=' . urlencode( $i ) . '" title="Go to page ' . $i . '">' . $i . '</a>' );
					}
					else {
						echo( '<strong>' . $i . '</strong>' );
					}
					echo( ' ' );
				}
				?>
			</p>
			<p class="right">
				<a href="#top" title="Jump to top">&#8593; top &#8593;</a>
			</p>
			<div style="clear: both;"></div>
		</div>
		<?php
		else:
		?>
		<div class="navbar">
			<p class="left">
				<strong>Quicklinks:</strong>
				<a href="?action=latest" title="Jump to right now in log (can be bookmarked)">Now</a> <a href="#" onClick="javascript: return bookmark('<?php echo( preg_replace( '/\?.*$/', '', selfURL() ) ); ?>?action=latest', 'Latest logs for <?php echo( $channel ); ?> on <?php echo( $network ); ?>');" title="Bookmark &#34;Latest logs for <?php echo( $channel ); ?> on <?php echo( $network ); ?>&#34;"><img src="./image/star.png" alt="Bookmark star" title="Bookmark &#34;Latest logs for <?php echo( $channel ); ?> on <?php echo( $network ); ?>&#34;" style="height: 1em;" /></a>
			</p>
			<div style="clear: both;"></div>
		</div>
		<ul>
			<?php
				for( $timestamp = mktime( 0, 0, 0 ); $timestamp >= (mktime( 0, 0, 0 ) - 7 * 24 * 3600); $timestamp -= (24 * 3600) ) {
					if( $logDatabase->getLogLineCount( $timestamp, $timestamp + (24 * 3600) ) > 0) {
						echo( '<li><a href="?action=view&#38;date=' . urlencode( strftime( '%Y-%m-%d', $timestamp ) ) . '" title="View logs for ' . strftime( '%Y-%m-%d', $timestamp ) . '">' . strftime( '%Y-%m-%d', $timestamp ) . '</a></li>' );
					}
					else {
						echo( '<li>' . strftime( '%Y-%m-%d', $timestamp ) . '</li>' );
					}
				}
			?>
		</ul>
		<?php
		endif;
		?>
<!--	<p>
			<a href="http://ircstats.mroot.net/cs-studs/" title="View some channel statistics">Some channel statistics</a>
		</p>
-->
		<p id="footer">
			Web log viewer for logs created by <a href="http://zerobot.de/" title="zerobot IRC bot">zerobot</a>
			written by Martin Wegner. Copyright &#169; 2011 Martin Wegner. Optimized for any modern, standard-compliant web browser.
			<a href="http://validator.w3.org/check?uri=referer" title="Valid XHTML 1.0">Valid XHTML 1.0</a> |
			<a href="http://jigsaw.w3.org/css-validator/check/referer" title="Valid CSS">Valid CSS</a>
		</p>
	</body>
</html>
