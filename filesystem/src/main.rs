// SPDX-FileCopyrightText: 2025 Caleb Depatie
//
// SPDX-License-Identifier: BSD-3-Clause

use std::{io::Read, process::ExitCode};
use std::fs::File;
use std::path::Path;
use std::str;

use fuser::{mount2, Filesystem, MountOption};
use spdlog::prelude::*;
use sqlite;
use clap::{Parser, Args};
use toml;

#[derive(Parser)]
#[command(version, about, long_about = None)]
struct CliArgs {
    /// Folder to mount the LakeFS instance under
    mount_point: String,

    /// Set the config file location
    #[arg(short, long, default_value="/etc/lakefs.conf")]
    config: String,

    /// Run program in foreground rather than as a daemon
    #[arg(short, default_value_t = false)]
    foreground: bool,

    /// Use an in-memory database rather than a file db
    #[arg(long, default_value_t = false)]
    tempdb: bool,
}

fn main() -> ExitCode {
    spdlog::default_logger().set_level_filter(spdlog::LevelFilter::All);

    let args = CliArgs::parse();

    let db_path = if !args.tempdb {
        let path = Path::new(&args.config);
        
        let mut file = match File::open(&path) {
            Err(why) => {
                spdlog::critical!("Couldnt Open Config {}: {}", path.display(), why);
                return ExitCode::FAILURE;
            },
            Ok(file) => file,
        };

        let mut file_buf: Vec<u8> = vec![]; 

        let _ = file.read_to_end(&mut file_buf);

        let file_str = match str::from_utf8(&file_buf) {
            Err(why) => {
                spdlog::critical!("Couldnt Read Config {}: {}", path.display(), why);
                return ExitCode::FAILURE;
            }
            Ok(s) => s, 
        };

        let config = file_str.parse::<toml::Table>().unwrap();

        let dir = config["dir"].as_str().unwrap(); 

        dir.to_string()
    } else {
        spdlog::info!("Using in-memory database");
        ":memory:".to_string()
    };

    let connection = sqlite::open(db_path).unwrap();

    let db_creation_query = "
        CREATE TABLE tags (data_id INTEGER, tag_value TEXT);
        CREATE TABLE data (id INTEGER PRIMARY KEY, path TEXT);
    ";

    connection.execute(db_creation_query).unwrap();

    spdlog::trace!("Initializing LakeFS");

    let fs = Lakefs{
        db_con: connection,
    };

    let options = [MountOption::DefaultPermissions];

    mount2(fs, args.mount_point, &options);

    return ExitCode::SUCCESS;
}

struct Lakefs {
    db_con: sqlite::Connection,
}

impl Filesystem for Lakefs {

}