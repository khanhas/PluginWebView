$(function()
{
    var playerTrack = $("#player-track"), bgArtwork = $('#img-album-art'), bgArtworkUrl, albumName = $('#album-name'), trackName = $('#track-name'), albumArt = $('#album-art'), sArea = $('#s-area'), seekBar = $('#seek-bar'), trackTime = $('#track-time'), insTime = $('#ins-time'), sHover = $('#s-hover'), playPauseButton = $("#play-pause-button"),  i = playPauseButton.find('i'), tProgress = $('#current-time'), tTime = $('#track-length'), seekT, seekLoc, seekBarPos, cM, ctMinutes, ctSeconds, curMinutes, curSeconds, durMinutes, durSeconds, playProgress, bTime, nTime = 0, buffInterval = null, tFlag = false, albums = ['Dawn','Me & You','Electro Boy','Home','Proxy (Original Mix)'], trackNames = ['Skylike - Dawn','Alex Skrindo - Me & You','Kaaze - Electro Boy','Jordan Schor - Home','Martin Garrix - Proxy'], albumArtworks = ['_1','_2','_3','_4','_5'], trackUrl = ['https://raw.githubusercontent.com/himalayasingh/music-player-1/master/music/2.mp3','https://raw.githubusercontent.com/himalayasingh/music-player-1/master/music/1.mp3','https://raw.githubusercontent.com/himalayasingh/music-player-1/master/music/3.mp3','https://raw.githubusercontent.com/himalayasingh/music-player-1/master/music/4.mp3','https://raw.githubusercontent.com/himalayasingh/music-player-1/master/music/5.mp3'], playPreviousTrackButton = $('#play-previous'), playNextTrackButton = $('#play-next'), currIndex = -1;

    const mState = RainmeterAPI.getMeasure("MeasureState");
    const mTitle = RainmeterAPI.getMeasure("MeasureTitle");
    const mArtist = RainmeterAPI.getMeasure("MeasureArtist");
    const mCover = RainmeterAPI.getMeasure("MeasureAlbumArtAddress");
    const mPosition = RainmeterAPI.getMeasure("MeasurePosition");
    const mDuration = RainmeterAPI.getMeasure("MeasureDuration");
    const mProgress = RainmeterAPI.getMeasure("MeasureProgress");

    function playPause()
    {
        if(mState.getNumber() == 1)
        {
            playerTrack.addClass('active');
            albumArt.addClass('active');
            i.attr('class','fas fa-pause');
        }
        else
        {
            playerTrack.removeClass('active');
            albumArt.removeClass('active');
            albumArt.removeClass('buffering');
            i.attr('class','fas fa-play');
        }
    }

    	
	function showHover(event)
	{
		seekBarPos = sArea.offset(); 
		seekT = event.clientX - seekBarPos.left;
		seekLoc = mDuration.getNumber() * (seekT / sArea.outerWidth());
		
		sHover.width(seekT);
		
		cM = seekLoc / 60;
		
		ctMinutes = Math.floor(cM);
		ctSeconds = Math.floor(seekLoc - ctMinutes * 60);
		
		if( (ctMinutes < 0) || (ctSeconds < 0) )
			return;
		
        if( (ctMinutes < 0) || (ctSeconds < 0) )
			return;
		
		if(ctMinutes < 10)
			ctMinutes = '0'+ctMinutes;
		if(ctSeconds < 10)
			ctSeconds = '0'+ctSeconds;
        
        if( isNaN(ctMinutes) || isNaN(ctSeconds) )
            insTime.text('--:--');
        else
		    insTime.text(ctMinutes+':'+ctSeconds);
            
		insTime.css({'left':seekT,'margin-left':'-21px'}).fadeIn(0);
	}

    function hideHover()
	{
        sHover.width(0);
        insTime.text('00:00').css({'left':'0px','margin-left':'0px'}).fadeOut(0);		
    }
    
    function playFromClickedPos()
    {
        mState.command("SetPosition " + (seekLoc / mDuration.getNumber() * 100));
		seekBar.width(seekT);
		hideHover();
    }

    function initPlayer()
	{	
		sArea.mousemove(function(event){ showHover(event); });
        sArea.mouseout(hideHover);
        
        sArea.on('click',playFromClickedPos);
        trackTime.addClass('active');

        setInterval(() => {
            playPause()
            albumName.text(mTitle.getString());
            trackName.text(mArtist.getString());
            // albumArt.find('img.active').removeClass('active');
            bgArtwork.attr("src", mCover.getString());
            seekBar.width(mProgress.getNumber() + "%");
            tProgress.text(mPosition.getString());
            tTime.text(mDuration.getString());
        }, 100);

        playPreviousTrackButton.on('click',function(){ mState.command("Previous")} );
        playNextTrackButton.on('click',function(){ mState.command("Next")});
		playPauseButton.on('click', function(){ mState.command("PlayPause")} );
	}
    
	initPlayer();
});