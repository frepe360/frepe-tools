#!/usr/bin/perl

use File::Basename;

my $rootUrl = "http://64.197.250.158/hls/envivio/nbc/";
my $manifestFile = "index.m3u8";

system( "wget -q \"$rootUrl/$manifestFile\" -O $manifestFile" );

while( 1 ) {
    open manifestFile, "<", $manifestFile or die("Couldn't open file " . $manifestFile);
    foreach $line (<manifestFile>) {
        if( $line =~ m|(.*\.m3u8)| ) {
            my $subManifestFile = $1;
            print "Get $subManifestFile\n";
            system( "wget -q $rootUrl/$subManifestFile -O $subManifestFile" );
            
            open subManifestFile, "<", $subManifestFile or die("Couldn't open file $subManifestFile");
            
            foreach $subLine (<subManifestFile>) {
                if( $subLine =~ m/^(.*\.ts).*$/ ) {
                    my $path = $1;
                    my $dir = dirname($path);
                    my $file = basename($path);
                    
                    ## Create directory if necessary
                    if( ($dir ne "") && (! -d $dir) ) {
                        system( "mkdir -p $dir" );
                    }
                    
                    ## Download new files
                    if( ! -e $path ) {
                        print "  Get $path\n";
                        system( "wget -q \"$rootUrl/$path\" -O \"$path\"" );
                    } else {
                        print "  Ign $path\n";
                    }
                }
            }
        }
    }
}
