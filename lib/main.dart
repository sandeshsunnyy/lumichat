import 'package:flutter/material.dart';
import 'package:torch_light/torch_light.dart';
import 'dart:async';

void main() => runApp(MyApp());

class MyApp extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      home: LiFiTransmit(),
    );
  }
}

class LiFiTransmit extends StatefulWidget {
  @override
  _LiFiTransmitState createState() => _LiFiTransmitState();
}

class _LiFiTransmitState extends State<LiFiTransmit> {
  TextEditingController _controller = TextEditingController();
  bool _isTransmitting = false;

  void startTransmission() async {
    String message = _controller.text;
    if (message.isEmpty) {
      return;
    }

    String binaryMessage = stringToBinary(message);

    // Add start signal and end sequences
    String startSignal = "1111";
    String endSignal = "11110000";
    String fullMessage = startSignal + binaryMessage + endSignal;

    setState(() {
      _isTransmitting = true;
    });

    await transmitBinaryMessage(fullMessage);

    setState(() {
      _isTransmitting = false;
    });
  }

  String stringToBinary(String input) {
    return input.codeUnits.map((int char) {
      return char.toRadixString(2).padLeft(8, '0');
    }).join();
  }


  Future<void> transmitBinaryMessage(String binaryMessage) async {
    int flashDuration = 150; //

    for (int i = 0; i < binaryMessage.length; i++) {
      String bit = binaryMessage[i];

      if (bit == '1') {
        await TorchLight.enableTorch(); // Turn flashlight ON for '1'
        await Future.delayed(Duration(milliseconds: flashDuration)); // Keep it ON for specified duration
      } else {
        await TorchLight.disableTorch(); // Turn flashlight OFF for '0'
        await Future.delayed(Duration(milliseconds: flashDuration));
      }
    }
    await TorchLight.disableTorch();
  }

  @override
  void dispose() {
    _controller.dispose();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    return SafeArea(
      child: Scaffold(
        appBar: AppBar(
          backgroundColor: Colors.deepPurple[800],
          title: Text('LiFi Data Transmitter', style: TextStyle(color: Colors.white, fontWeight: FontWeight.bold),),
        ),
        backgroundColor: Colors.deepPurple[400],
        body: Padding(
          padding: const EdgeInsets.all(16.0),
          child: Column(
            mainAxisAlignment: MainAxisAlignment.center,
            children: [
              Text('LumiCHAT',
              style: TextStyle(fontSize: 35.0, color: Colors.white, fontWeight: FontWeight.bold),),
              SizedBox(height: 20),
              Text('Chat with light!',style: TextStyle(fontSize: 25.0, color: Colors.white70),),
              SizedBox(height: 40),
              TextField(
                controller: _controller,
                decoration: InputDecoration(labelText: 'Enter Message',
                  border: OutlineInputBorder(
                    borderRadius: BorderRadius.circular(20.0),
                    borderSide: BorderSide(color: Colors.grey),
                  ),
                floatingLabelBehavior: FloatingLabelBehavior.never,
                filled: true,
                fillColor: Colors.white,
                  focusColor: Colors.white,
              ),),
              SizedBox(height: 20),
              ElevatedButton(
                onPressed: _isTransmitting ? null : startTransmission,
                child: Text(_isTransmitting ? 'Transmitting...' : 'Transmit'),
                style: ButtonStyle(backgroundColor: WidgetStateProperty.all<Color>(Colors.white)),
              ),
            ],
          ),
        ),
      ),
    );
  }
}