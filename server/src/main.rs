use std::io::{self, Read, Write};
use std::mem::size_of;
use std::net::{SocketAddr, TcpListener, TcpStream};
use std::os::raw::c_char;
use std::sync::{Arc, Mutex};
use std::thread;
#[derive(Debug, PartialEq)]

struct Player {
    id: u64, // int 64 on client I know
    x: f32,
    y: f32,
    z: f32,
    rotation_x: f32,
    rotation_y: f32,
    rotation_z: f32,
}

struct UpdateClient {
    total_players: i32,
    players: [Player],
}

/*

Index 0 - Type Enum:
0 = Introduce new client
    i32 ID

1 = Remove client
    i32 ID

2 = Client transformation
    i32 ID
    f32 x
    f32 y
    f32 z

*/

#[allow(unreachable_code)]
fn handle_client(mut stream: TcpStream, clients: Arc<Mutex<Vec<Player>>>) -> io::Result<()> {
    let peer_addr = stream.peer_addr()?;
    let client_player_id = hash_ip_address(&peer_addr);
    println!("New connection: {}", peer_addr);

    let client = Player {
        id: client_player_id,
        x: 0.0,
        y: 0.0,
        z: 0.0,
        rotation_x: 0.0,
        rotation_y: 0.0,
        rotation_z: 0.0,
    };
    {
        clients.lock().unwrap().push(client);
        println!("Client ID: {}", clients.lock().unwrap().len());
    }

    let mut buffer = [0u8; 1024]; // or any appropriate size, ensuring it's at least 8 bytes
    let offset = 0; // Starting at the beginning of the buffer
    buffer[offset..(offset + 8)].copy_from_slice(&client_player_id.to_le_bytes()); // Copying the hashedID
    stream.write_all(&buffer[..8])?; // Sending the first 8 bytes

    loop {
        //buffer.fill(0);

        println!("Connection ");
        let nbytes = match stream.read(&mut buffer) {
            Ok(n) => n,
            Err(_) => {
                println!("Connection closed XD");
                break;
            }
        };
        let recv_type: [u8; 4] = buffer[0..4].try_into().expect("slice with incorrect length");
        //let player_offset = 4;
        let player_id: [u8; 8] = buffer[4..12].try_into().expect("slice with incorrect length");
        let id = u64::from_le_bytes(player_id); // Now correctly interpreting as u64

        // Then, parse each float value for the player's position
        let pos_x: [u8; 4] = buffer[12..16].try_into().expect("slice with incorrect length");
        let pos_y: [u8; 4] = buffer[16..20].try_into().expect("slice with incorrect length");
        let pos_z: [u8; 4] = buffer[20..24].try_into().expect("slice with incorrect length");

        // Convert bytes to their respective values
        let x = f32::from_le_bytes(pos_x);
        let y = f32::from_le_bytes(pos_y);
        let z = f32::from_le_bytes(pos_z);

        let rot_x: [u8; 4] = buffer[24..28].try_into().expect("slice with incorrect length");
        let rot_y: [u8; 4] = buffer[28..32].try_into().expect("slice with incorrect length");
        let rot_z: [u8; 4] = buffer[32..36].try_into().expect("slice with incorrect length");

        let rotation_x = f32::from_le_bytes(rot_x);
        let rotation_y = f32::from_le_bytes(rot_y);
        let rotation_z = f32::from_le_bytes(rot_z);
        println!(
            "Received: RECEVE TYPE: {:?}\n PLAYER ID;{:?} POSITION: {:?} {:?} {:?}\n ROTATION: {:?} {:?} {:?}\n",
            recv_type, id, x, y, z, rotation_x, rotation_y, rotation_z
        );
        {
            let mut clients_lock = clients.lock().unwrap();
            println!("clients size: {}", clients_lock.len());
            let client = match clients_lock.iter_mut().find(|player| player.id == client_player_id) {
                Some(client) => client,
                None => {
                    break;
                }
            };

            client.x = x;
            client.y = y;
            client.z = z;

            client.rotation_x = rotation_x;
            client.rotation_y = rotation_y;
            client.rotation_z = rotation_z;
        }

        let buffer_offset = construct_client_update(&mut buffer, clients.clone());
        ///println!("Sent update to client: {}", buffer_offset);
        //println!("Buffer bytes: {:?}", buffer);
        stream.write_all(&buffer[..buffer_offset])?;
        //println!("LOOP DONE ");
    }
    let mut clients_lock = clients.lock().unwrap();
    clients_lock.retain(|player| player.id != client_player_id);
    for player in clients_lock.iter() {
        println!("PLAYER{:?}", player);
    }
    println!("Exiting client thread for client: {}", client_player_id);
    Ok(())
}

fn main() -> io::Result<()> {
    // let mut ip_buffer = String::new();
    // let mut port_buffer = String::new();

    // println!("Enter machine IP address:");
    // std::io::stdin().read_line(&mut ip_buffer).unwrap();
    // println!("Enter machine IP address port:");
    // std::io::stdin().read_line(&mut port_buffer).unwrap();
    // println!("Connecting to {} on port {}", ip_buffer, port_buffer);
    // return Ok(());

    let listener = TcpListener::bind("192.168.38.164:6969")?;
    println!("Server listening on port 6969");

    let clients: Arc<Mutex<Vec<Player>>> = Arc::new(Mutex::new(Vec::new()));

    for stream in listener.incoming() {
        let clients_clone = clients.clone();
        println!("Active connections: {}", clients_clone.lock().unwrap().len());
        match stream {
            Ok(stream) => {
                thread::spawn(move || {
                    handle_client(stream, clients_clone);
                });
            }
            Err(e) => eprintln!("Failed to accept connection: {}", e),
        }
    }

    Ok(())
}

fn construct_client_update(buffer: &mut [u8], clients: Arc<Mutex<Vec<Player>>>) -> usize {
    let clients_lock = clients.lock().unwrap();

    let mut offset = 0;

    let type_enum: i32 = 1;
    buffer[offset..(offset + 4)].copy_from_slice(&type_enum.to_le_bytes());
    offset += 4;

    //println!("{}", clients_lock.len());
    let number_of_players: i32 = clients_lock.len() as i32;
    //println!("Number of players: {}", number_of_players);
    buffer[offset..(offset + 4)].copy_from_slice(&number_of_players.to_le_bytes());
    offset += 4;

    for player in clients_lock.iter() {
        println!("Sending update for player {:?}", player);
        buffer[offset..(offset + 8)].copy_from_slice(&player.id.to_le_bytes());
        offset += 8;

        buffer[offset..(offset + 4)].copy_from_slice(&player.x.to_le_bytes());
        offset += 4;

        buffer[offset..(offset + 4)].copy_from_slice(&player.y.to_le_bytes());
        offset += 4;

        buffer[offset..(offset + 4)].copy_from_slice(&player.z.to_le_bytes());
        offset += 4;

        buffer[offset..(offset + 4)].copy_from_slice(&player.rotation_x.to_le_bytes());
        offset += 4;

        buffer[offset..(offset + 4)].copy_from_slice(&player.rotation_y.to_le_bytes());
        offset += 4;

        buffer[offset..(offset + 4)].copy_from_slice(&player.rotation_z.to_le_bytes());
        offset += 4;
    }

    println!("BUffer size: {:?}", buffer.len());
    println!("Offset: {:?}", offset);
    offset // Return the total size of the constructed message
}

fn hash_ip_address(ip_address: &SocketAddr) -> u64 {
    let mut hasher = std::collections::hash_map::DefaultHasher::new();
    std::hash::Hash::hash(&ip_address, &mut hasher);
    std::hash::Hasher::finish(&hasher)
}
