package HealInput;

import java.awt.Color;
import java.awt.FileDialog;
import java.awt.Toolkit;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyEvent;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;
import java.net.URL;

import javax.imageio.ImageIO;
import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.KeyStroke;


/**
 *  This class provides methods for manipulating individual pixels of
 *  an image. The original image can be read from a <tt>.jpg</tt>, <tt>.gif</tt>,
 *  or <tt>.png</tt> file or the user can create a blank image of a given size.
 *  This class includes methods for displaying the image in a window on
 *  the screen or saving it to a file.
 *  <p>
 *  Pixel (<em>x</em>, <em>y</em>) is column <em>x</em> and row <em>y</em>.
 *  By default, the origin (0, 0) is upper left, which is a common convention
 *  in image processing.
 *  The method <tt>setOriginLowerLeft()</tt> change the origin to the lower left.
 *  <p>
 *  For additional documentation, see
 *  <a href="http://introcs.cs.princeton.edu/31datatype">Section 3.1</a> of
 *  <i>Introduction to Programming in Java: An Interdisciplinary Approach</i>
 *  by Robert Sedgewick and Kevin Wayne.
 *
 *  @author Robert Sedgewick
 *  @author Kevin Wayne
 */
public final class Picture implements ActionListener {
    private BufferedImage image;               // the rasterized image
    private JFrame frame;                      // on-screen view
    private String filename;                   // name of file
    private boolean isOriginUpperLeft = true;  // location of origin
    private final int width, height;           // width and height

   /**
     * Initializes a blank <tt>width</tt>-by-<tt>height</tt> picture, with <tt>width</tt> columns
     * and <tt>height</tt> rows, where each pixel is black.
     *
     * @param width the width of the picture
     * @param height the height of the picture
     */
    public Picture(int width, int height) {
        if (width  < 0) throw new IllegalArgumentException("width must be nonnegative");
        if (height < 0) throw new IllegalArgumentException("height must be nonnegative");
        this.width  = width;
        this.height = height;
        image = new BufferedImage(width, height, BufferedImage.TYPE_INT_RGB);
        // set to TYPE_INT_ARGB to support transparency
        filename = width + "-by-" + height;
    }

   /**
     * Initializes a new picture that is a deep copy of the argument picture.
     *
     * @param picture the picture to copy
     */
    public Picture(Picture picture) {
        width  = picture.width();
        height = picture.height();
        image = new BufferedImage(width, height, BufferedImage.TYPE_INT_RGB);
        filename = picture.filename;
        for (int col = 0; col < width(); col++)
            for (int row = 0; row < height(); row++)
                image.setRGB(col, row, picture.get(col, row).getRGB());
    }

   /**
     * Initializes a picture by reading from a file or URL.
     *
     * @param filename the name of the file (.png, .gif, or .jpg) or URL.
     */
    public Picture(String filename) {
        this.filename = filename;
        try {
            // try to read from file in working directory
            File file = new File(filename);
            if (file.isFile()) {
                image = ImageIO.read(file);
            }

            // now try to read from file in same directory as this .class file
            else {
                URL url = getClass().getResource(filename);
                if (url == null) {
                    url = new URL(filename);
                }
                image = ImageIO.read(url);
            }
            width  = image.getWidth(null);
            height = image.getHeight(null);
        }
        catch (IOException e) {
            // e.printStackTrace();
            throw new RuntimeException("Could not open file: " + filename);
        }
    }

   /**
     * Initializes a picture by reading in a .png, .gif, or .jpg from a file.
     *
     * @param file the file
     */
    public Picture(File file) {
        try {
            image = ImageIO.read(file);
        }
        catch (IOException e) {
            e.printStackTrace();
            throw new RuntimeException("Could not open file: " + file);
        }
        if (image == null) {
            throw new RuntimeException("Invalid image file: " + file);
        }
        width  = image.getWidth(null);
        height = image.getHeight(null);
        filename = file.getName();
    }

   /**
     * Returns a JLabel containing this picture, for embedding in a JPanel,
     * JFrame or other GUI widget.
     *
     * @return the <tt>JLabel</tt>
     */
    public JLabel getJLabel() {
        if (image == null) return null;         // no image available
        ImageIcon icon = new ImageIcon(image);
        return new JLabel(icon);
    }

   /**
     * Sets the origin to be the upper left pixel. This is the default.
     */
    public void setOriginUpperLeft() {
        isOriginUpperLeft = true;
    }

   /**
     * Sets the origin to be the lower left pixel.
     */
    public void setOriginLowerLeft() {
        isOriginUpperLeft = false;
    }

   /**
     * Displays the picture in a window on the screen.
     */
    public void show() {

        // create the GUI for viewing the image if needed
        if (frame == null) {
            frame = new JFrame();

            JMenuBar menuBar = new JMenuBar();
            JMenu menu = new JMenu("File");
            menuBar.add(menu);
            JMenuItem menuItem1 = new JMenuItem(" Save...   ");
            menuItem1.addActionListener(this);
            menuItem1.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_S,
                                     Toolkit.getDefaultToolkit().getMenuShortcutKeyMask()));
            menu.add(menuItem1);
            frame.setJMenuBar(menuBar);



            frame.setContentPane(getJLabel());
            // f.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
            frame.setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);
            frame.setTitle(filename);
            frame.setResizable(false);
            frame.pack();
            frame.setVisible(true);
        }

        // draw
        frame.repaint();
    }

   /**
     * Returns the height of the picture.
     *
     * @return the height of the picture (in pixels)
     */
    public int height() {
        return height;
    }

   /**
     * Returns the width of the picture.
     *
     * @return the width of the picture (in pixels)
     */
    public int width() {
        return width;
    }

   /**
     * Returns the color of pixel (<tt>col</tt>, <tt>row</tt>).
     *
     * @param col the column index
     * @param row the row index
     * @return the color of pixel (<tt>col</tt>, <tt>row</tt>)
     * @throws IndexOutOfBoundsException unless both 0 &le; <tt>col</tt> &lt; <tt>width</tt>
     *         and 0 &le; <tt>row</tt> &lt; <tt>height</tt>
     */
    public Color get(int col, int row) {
        if (col < 0 || col >= width())  throw new IndexOutOfBoundsException("col must be between 0 and " + (width()-1));
        if (row < 0 || row >= height()) throw new IndexOutOfBoundsException("row must be between 0 and " + (height()-1));
        if (isOriginUpperLeft) return new Color(image.getRGB(col, row));
        else                   return new Color(image.getRGB(col, height - row - 1));
    }

   /**
     * Sets the color of pixel (<tt>col</tt>, <tt>row</tt>) to given color.
     *
     * @param col the column index
     * @param row the row index
     * @param color the color
     * @throws IndexOutOfBoundsException unless both 0 &le; <tt>col</tt> &lt; <tt>width</tt>
     *         and 0 &le; <tt>row</tt> &lt; <tt>height</tt>
     * @throws NullPointerException if <tt>color</tt> is <tt>null</tt>
     */
    public void set(int col, int row, Color color) {
        if (col < 0 || col >= width())  throw new IndexOutOfBoundsException("col must be between 0 and " + (width()-1));
        if (row < 0 || row >= height()) throw new IndexOutOfBoundsException("row must be between 0 and " + (height()-1));
        if (color == null) throw new NullPointerException("can't set Color to null");
        if (isOriginUpperLeft) image.setRGB(col, row, color.getRGB());
        else                   image.setRGB(col, height - row - 1, color.getRGB());
    }

   /**
     * Returns true if this picture is equal to the argument picture.
     *
     * @param other the other picture
     * @return <tt>true</tt> if this picture is the same dimension as <tt>other</tt>
     *         and if all pixels have the same color; <tt>false</tt> otherwise
     */
    public boolean equals(Object other) {
        if (other == this) return true;
        if (other == null) return false;
        if (other.getClass() != this.getClass()) return false;
        Picture that = (Picture) other;
        if (this.width()  != that.width())  return false;
        if (this.height() != that.height()) return false;
        for (int col = 0; col < width(); col++)
            for (int row = 0; row < height(); row++)
                if (!this.get(col, row).equals(that.get(col, row))) return false;
        return true;
    }

    /**
     * This operation is not supported because pictures are mutable.
     *
     * @return does not return a value
     * @throws UnsupportedOperationException if called
     */
    public int hashCode() {
        throw new UnsupportedOperationException("hashCode() is not supported because pictures are mutable");
    }

   /**
     * Saves the picture to a file in a standard image format.
     * The filetype must be .png or .jpg.
     *
     * @param name the name of the file
     */
    public void save(String name) {
        save(new File(name));
    }

   /**
     * Saves the picture to a file in a PNG or JPEG image format.
     *
     * @param file the file
     */
    public void save(File file) {
        filename = file.getName();
        if (frame != null) frame.setTitle(filename);
        String suffix = filename.substring(filename.lastIndexOf('.') + 1);
        suffix = suffix.toLowerCase();
        if (suffix.equals("jpg") || suffix.equals("png")) {
            try {
                ImageIO.write(image, suffix, file);
            }
            catch (IOException e) {
                e.printStackTrace();
            }
        }
        else {
            System.out.println("Error: filename must end in .jpg or .png");
        }
    }

   /**
     * Opens a save dialog box when the user selects "Save As" from the menu.
     */
    @Override
    public void actionPerformed(ActionEvent e) {
        FileDialog chooser = new FileDialog(frame,
                             "Use a .png or .jpg extension", FileDialog.SAVE);
        chooser.setVisible(true);
        if (chooser.getFile() != null) {
            save(chooser.getDirectory() + File.separator + chooser.getFile());
        }
    }


   /**
     * Unit tests this <tt>Picture</tt> data type.
     * Reads a picture specified by the command-line argument,
     * and shows it in a window on the screen.
     */
    public static void main(String[] args) {
        Picture picture = new Picture(args[0]);
        System.out.printf("%d-by-%d\n", picture.width(), picture.height());
        picture.show();
    }

    
    public Picture swirl(int amount)
    {
    	int picW  = this.width();
	    int picH = this.height();
	    double x0 = 0.5 * (picW - 1);
	    double y0 = 0.5 * (picH - 1);
    	
    	Picture swPic = new Picture(picW, picH);
    
    	//colora di bianco l'immagine d' appoggio
        for (int sx = 0; sx < picW; sx++) 
        	for (int sy = 0; sy < picH; sy++) 
        		swPic.set(sx, sy, Color.WHITE);
    	
        for (int sx = 0; sx < picW; sx++) 
        {
            for (int sy = 0; sy < picH; sy++) 
            {
                double dx = sx - x0;
                double dy = sy - y0;
                double r = Math.sqrt(dx*dx + dy*dy);
                double angle = Math.PI / amount * r;
                int tx = (int) (+dx * Math.cos(angle) - dy * Math.sin(angle) + x0);
                int ty = (int) (+dx * Math.sin(angle) + dy * Math.cos(angle) + y0);

                // plot pixel (sx, sy) the same color as (tx, ty) if it's in bounds
	            if (tx >= 0 && tx < picW && ty >= 0 && ty < picH)
	            	swPic.set(sx, sy, this.get(tx, ty));
	        }
	    }

    	return swPic;
    }
    
    
    
    public Picture flipX()
    {
    	int picW  = this.width();
	    int picH = this.height();

	    Picture fxPic = new Picture(picW, picH);
	    
	    for (int y = 0; y < picH; y++) 
	    {
	    	for (int x = 0; x < picW / 2; x++) 
	    	{
	    		Color c1 = this.get(x, y);
	    		Color c2 = this.get(picW - x - 1, y);
	    		fxPic.set(x, y, c2);
	    		fxPic.set(picW - x - 1, y, c1);
	    	}
	    }
    	
    	return fxPic; 
    }
    
    public Picture flipY()
    {
    	int picW  = this.width();
	    int picH = this.height();

	    Picture fyPic = new Picture(picW, picH);
	    
	    for (int y = 0; y < picH / 2; y++) 
	    {
	    	for (int x = 0; x < picW ; x++) 
	    	{
	    		Color c1 = this.get(x, y);
	    		Color c2 = this.get(x, picH - y - 1);
	    		fyPic.set(x, y, c2);
	    		fyPic.set(x, picH - y - 1, c1);
	    	}
	    }
    	
    	return fyPic; 
    }
    
    
    
    public Picture rotate(double angle)
    {
    
               
    	int picW  = this.width();
	    int picH = this.height();
	    
        double rad = Math.toRadians(angle);
        
        double sin = Math.sin(rad);
        double cos = Math.cos(rad);
        double x0 = 0.5 * (picW  - 1);     // point to rotate about
        double y0 = 0.5 * (picH - 1);     // center of image

        Picture rPic = new Picture(picW, picH);

        // rotation
        for (int x = 0; x < picW; x++) 
        {
            for (int y = 0; y < picH; y++) 
            {
                double a = x - x0;
                double b = y - y0;
                int xx = (int) (+a * cos - b * sin + x0);
                int yy = (int) (+a * sin + b * cos + y0);

                // plot pixel (x, y) the same color as (xx, yy) if it's in bounds
                if (xx >= 0 && xx < picW && yy >= 0 && yy < picH) 
                {
                    rPic.set(x, y, this.get(xx, yy));
                }
            }
        }
        
    	return rPic;
    	
    }
    
    
    public Picture wave()
    {
      
    	int picW  = this.width();
    	int picH = this.height();

    	Picture wPic = new Picture(picW, picH);

    	//colora di bianco l'immagine d' appoggio
        for (int sx = 0; sx < picW; sx++) 
        	for (int sy = 0; sy < picH; sy++) 
        		wPic.set(sx, sy, Color.WHITE);
        
    	// wave filter
    	for (int x = 0; x < picW; x++) 
    	{
    		for (int y = 0; y < picH; y++) 
    		{
    			int xx = x;
    			int yy = (int) (y + 20 * Math.sin(x * 2 * Math.PI / 2));
    			if (yy >= 0 && yy < picH) 
    			{
    				wPic.set(x, y, this.get(xx, yy));
    			}
    		}
    	}

    	return wPic;
	}
    
    public Picture scale(int newW, int newH)
    {
    	
    	Picture sPic = new Picture(newW, newH);	
    	
   
    	for (int tx = 0; tx < newW; tx++) 
    	{
    		for (int ty = 0; ty < newH; ty++) 
    		{
    			int sx = tx * this.width()  / newW;
    			int sy = ty * this.height() / newH;
    			Color color = this.get(sx, sy);
    			sPic.set(tx, ty, color);
            }
        }

    	return sPic;
    	
   
    }

    
    public Picture threshold(int t)
    {
    	
    	int picW  = this.width();
    	int picH = this.height();

    	Picture tPic = new Picture(picW, picH);

    	
    	for (int i=0; i<this.width(); i++) 
    	{
        	for (int j=0; j<this.height(); j++)
        	{
        		Color color = this.get(i, j);
        		double lum = luminance(color);
        		if(lum>=t) 
        			tPic.set(i, j, Color.WHITE);
                else            
                	tPic.set(i, j, Color.BLACK);
        	}
    		
    	}
    	
    	return tPic;
    }
    
    
    
    
    
    
    
    // return the monochrome luminance of given color
    public static double luminance(Color color) 
    {
        int r = color.getRed();
        int g = color.getGreen();
        int b = color.getBlue();
        return .299*r + .587*g + .114*b;
    }
    

    
    public Picture centralizeAndEnlarge()
    {

    	int subX=-1, subY=-1;
    	int subW=-1, subH=-1;
   	
    	
    	//check if filtered 
    	for (int i=0; i<this.width(); i++) 
    	{
        	for (int j=0; j<this.height(); j++)
        	{
        		Color color = this.get(i, j);
        		
        		if(!color.equals(Color.BLACK) && !color.equals(Color.WHITE))
        		{
        			System.out.println("Error: the image is not filtered!");
        			return this;
        		}
 
        	}
    	}
    	
    	
    	//scan colums from left
    	for (int i=0; i<this.width(); i++) 
    	{
        	for (int j=0; j<this.height(); j++)
        	{
        		Color color = this.get(i, j);
        		if(color.equals(Color.BLACK))
        		{
        			subX=i;
        			break;
        		}
        	}
        	if(subX!=-1)
        		break;
    	}
    	if(subX==-1)
    	{
    		System.out.println("Error: no black pixel found!");
			return this;
    	}
    	
    	//scan colums from right
    	for (int i=this.width()-1; i>=0; i--) 
    	{
        	for (int j=0; j<this.height(); j++)
        	{
        		Color color = this.get(i, j);
        		if(color.equals(Color.BLACK))
        		{
        			subW=i-subX;
        			break;
        		}
        	}
        	if(subW!=-1)
        		break;
        	
    	}

    	
    	//scan rows from above
    	for (int i=0; i<this.height(); i++) 
    	{
        	for (int j=0; j<this.width(); j++)
        	{
        		Color color = this.get(j, i);
        		if(color.equals(Color.BLACK))
        		{
        			subY=i;
        			break;
        		}
        	}
        	if(subY!=-1)
       		break;
    	}
    	
    	//scan rows from bottom
    	for (int i=this.height()-1; i>=0; i--) 
    	{
        	for (int j=0; j<this.width(); j++)
        	{
        		Color color = this.get(j, i);
        		if(color.equals(Color.BLACK))
        		{
        			subH=i-subY;
        			break;
        		}
        	}
        	if(subH!=-1)
        		break;

    	}
    	
    	
    	Picture sub_cPic = new Picture(subW,subH);    	    	
    	sub_cPic.image = this.image.getSubimage(subX, subY, subW, subH); 
    	
    	int cPicSize = (sub_cPic.width() <= sub_cPic.height()) ? (sub_cPic.height()+sub_cPic.height()/5) : (sub_cPic.width()+sub_cPic.width()/5);
    	Picture cPic = new Picture(cPicSize, cPicSize);
    	
    	int offsetX = (cPicSize - subW) / 2;
    	int offsetY = (cPicSize - subH) / 2;
    	
    	
    	for (int i=0; i<cPicSize; i++) 
    	{
        	for (int j=0; j<cPicSize; j++)
        	{
        		if(i>offsetX && i<offsetX+subW && j>offsetY && j<offsetY+subH)
        		{
        			Color color = sub_cPic.get(i-offsetX, j-offsetY);
            		cPic.set(i, j, color);
        		}
        		else
        			cPic.set(i, j, Color.WHITE);
        	}
    	}
   	
    	return cPic;
    }
    
    
    public Picture centralize()
    {
    	
    	int subX=-1, subY=-1;
    	int subW=-1, subH=-1;
   	
    	
    	//check if filtered 
    	for (int i=0; i<this.width(); i++) 
    	{
        	for (int j=0; j<this.height(); j++)
        	{
        		Color color = this.get(i, j);
        		
        		if(!color.equals(Color.BLACK) && !color.equals(Color.WHITE))
        		{
        			System.out.println("Error: the image is not filtered!");
        			return this;
        		}
 
        	}
    	}
    	
    	
    	//scan colums from left
    	for (int i=0; i<this.width(); i++) 
    	{
        	for (int j=0; j<this.height(); j++)
        	{
        		Color color = this.get(i, j);
        		if(color.equals(Color.BLACK))
        		{
        			subX=i;
        			break;
        		}
        	}
        	if(subX!=-1)
        		break;
    	}
    	if(subX==-1)
    	{
    		System.out.println("Error: no black pixel found!");
			return this;
    	}
    	
    	//scan colums from right
    	for (int i=this.width()-1; i>=0; i--) 
    	{
        	for (int j=0; j<this.height(); j++)
        	{
        		Color color = this.get(i, j);
        		if(color.equals(Color.BLACK))
        		{
        			subW=i-subX;
        			break;
        		}
        	}
        	if(subW!=-1)
        		break;
        	
    	}
    	   	
    	
    	//scan rows from above
    	for (int i=0; i<this.height(); i++) 
    	{
        	for (int j=0; j<this.width(); j++)
        	{
        		Color color = this.get(j, i);
        		if(color.equals(Color.BLACK))
        		{
        			subY=i;
        			break;
        		}
        	}
       	if(subY!=-1)
       		break;
    	}
    	
    	//scan rows from bottom
    	for (int i=this.height()-1; i>=0; i--) 
    	{
        	for (int j=0; j<this.width(); j++)
        	{
        		Color color = this.get(j, i);
        		if(color.equals(Color.BLACK))
        		{
        			subH=i-subY;
        			break;
        		}
            	if(subH!=-1)
            		break;
        	}
    	}
    	
    	
    	Picture sub_cPic = new Picture(subW,subH);    	    	
    	sub_cPic.image = this.image.getSubimage(subX, subY, subW, subH); 

    	
    	Picture cPic = new Picture(this.width(),  this.height());
    	int offsetX = (this.width() - subW) / 2;
    	int offsetY = (this.height() - subH) / 2;
    	
    	
    	for (int i=0; i<this.width(); i++) 
    	{
        	for (int j=0; j<this.height(); j++)
        	{
        		if(i>offsetX && i<offsetX+subW && j>offsetY && j<offsetY+subH)
        		{
        			Color color = sub_cPic.get(i-offsetX, j-offsetY);
            		cPic.set(i, j, color);
        		}
        		else
        			cPic.set(i, j, Color.WHITE);
        	}
    	}
   	
    	return cPic;
    }
    
    
    public Picture detectEdges()
    {
    	int[][] filter1 = {{ -1,  0,  1 },{ -2,  0,  2 },{ -1,  0,  1 }};
        int[][] filter2 = {{  1,  2,  1 },{  0,  0,  0 },{ -1, -2, -1 }};

        int width    = this.width();
    	int height   = this.height();
    	Picture ePic = new Picture(width, height);


    	for (int y = 1; y < height - 1; y++) 
    	{
    		for (int x = 1; x < width - 1; x++) 
    		{

    	             
    			int[][] gray = new int[3][3];
    			for (int i = 0; i < 3; i++) 
    			{
    				for (int j = 0; j < 3; j++) 
    	            {
    	            	Color color = this.get(x-1+i, y-1+j);
    	            	double lum = luminance(color);
    	                gray[i][j] = (int) lum;
    	            }
    			}

    	           
    			int gray1 = 0, gray2 = 0;
    	        for (int i = 0; i < 3; i++) 
    	        {
    	        	for (int j = 0; j < 3; j++) 
    	        	{
    	        		gray1 += gray[i][j] * filter1[i][j];
    	                gray2 += gray[i][j] * filter2[i][j];
    	        	}
    	        }
    	        
    	        int magnitude = 255 - truncate((int) Math.sqrt(gray1*gray1 + gray2*gray2));
    	        Color grayscale = new Color(magnitude, magnitude, magnitude);
    	        ePic.set(x, y, grayscale);
    		}
    	}
    	return ePic;
    }
    	
    public static int truncate(int a) 
    {
        if      (a <   0) return 0;
        else if (a > 255) return 255;
        else              return a;
    }
      
    void gen4distortedInputsFrom1(String path, int startIndex)
    {
		
    	Picture swirlPic = new Picture(this);
    	String name = path.substring(path.length()-2,path.length()-1);
    			
    	//i: degree of swirl
		for(int i=1000; i<=2000; i+=1000)
		{			
			swirlPic = this;
			swirlPic = swirlPic.swirl(i);
			swirlPic.save(path + name +" (" + startIndex + ").png");
			startIndex++;
			
			swirlPic = this;
			swirlPic = swirlPic.flipX();
			swirlPic = swirlPic.swirl(i);
			swirlPic = swirlPic.flipX();
			swirlPic.save(path + name +" (" + startIndex + ").png");
			startIndex++;
			
		}
    }
            
	
}
