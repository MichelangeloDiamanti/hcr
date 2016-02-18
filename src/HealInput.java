package HealInput;

import java.awt.Color;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;
import java.io.PrintWriter;
import java.net.URLDecoder;

public class HealInput {

	
	public static void main(String[] args) throws IOException
	{
		
		//ricava il path da dove parte il programma
		String encodedPath = HealInput.class.getProtectionDomain().getCodeSource().getLocation().getPath();
		String path = URLDecoder.decode(encodedPath, "UTF-8");
		String name = HealInput.class.getSimpleName();
		
		path = path.substring(0, path.length() - (9 + name.length()));
		String pathImg = path + "/tmp/realTimeTestLetter.png";		
		String pathTxt = path + "/tmp/realTimeTestLetter.txt";

		//istanzia una nuova immagine
		Picture pic = new Picture(pathImg);
		
		//applica il filtro threshold (soglia=128)
		pic = pic.threshold(128);
		//centra ed espande la lettera (i pixel neri)
		pic = pic.centralizeAndEnlarge();
		//ridimensiona l'immagine a 64 pixel
		pic = pic.scale(64, 64);
		//salva l' immagine
		pic.save(pathImg);	
		
		
		// printwriter per scrivere sul file di output l' immagine codificata
		PrintWriter writer;		
		writer = new PrintWriter(pathTxt);	// apre il file di output
					
		    
		BufferedImage img = javax.imageio.ImageIO.read(new File(pathImg));	// legge l'immagine
		int pixel = -1;		// contenuto (integer) di un pixel inizializzato a -1
		
		boolean blank = true;
		
		// scorre i pixel dell'immagine in altezza
		for(int i=0; i<img.getHeight(); i++)
		{
			// scorre i pixel dell'immagine in larghezza
			for(int j=0; j<img.getWidth(); j++)
			{
				pixel = img.getRGB(j,i);		// legge il contenuto di un pixel
					
				// ricava il colore del pixel e lo separa in RGB
				Color c = new Color(pixel);
				int R = c.getRed();
				int G = c.getGreen();
				int B = c.getBlue();
					
				if(R==G && G==B & 255 - R > 127)
				{
					writer.print("1 ");			// scriviamo il valore RGB nel file di output
					blank = false;
				}
				else
				{
					writer.print("0 ");		// scriviamo 0 nel file di output
				}
			}
			
		}
		writer.println();
			
		writer.close();
		
		//se il file è composto da tutti zeri (immagine bianca) allora il file conterrà solamente un -1
		if(blank)
		{
			PrintWriter bwriter;		// printwriter per scrivere sul file di output
			bwriter = new PrintWriter(pathTxt);	// apre il file di output
			bwriter.print("-1");
			bwriter.close();
		}

	}
		
}